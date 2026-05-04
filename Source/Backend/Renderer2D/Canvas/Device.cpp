#include "Pch.hpp"
#include "Device.hpp"

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Cache.hpp"

// 3. WIL
#include <wil/result_macros.h>

namespace N503::Renderer2D::Canvas
{
    Device::Device()
    {
        InitializeDirectX();

        // デバイス初期化後にキャッシュを作成[cite: 16]
        m_ResourceCache = std::make_unique<Canvas::Cache>(*this);
    }

    Device::~Device() = default;

    auto Device::InitializeDirectX() -> void
    {
        // 1. D2D および DWrite ファクトリの作成
        D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        THROW_IF_FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &options, m_D2DFactory.put_void()));
        THROW_IF_FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), m_DWriteFactory.put_unknown()));

        // 2. Direct3D11 デバイスの作成[cite: 7]
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

        THROW_IF_FAILED(::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            m_D3DDevice.put(),
            nullptr,
            m_D3DContext.put()
        ));

        // 3. DXGI デバイス経由で Direct2D デバイスを作成[cite: 7, 9]
        auto dxgiDevice = m_D3DDevice.query<IDXGIDevice>();
        THROW_IF_FAILED(m_D2DFactory->CreateDevice(dxgiDevice.get(), m_D2DDevice.put()));

        // 4. デバイスコンテキストの作成とアップグレード
        wil::com_ptr<ID2D1DeviceContext> baseContext;
        THROW_IF_FAILED(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, baseContext.put()));

        // 前回の修正に基づき、QueryInterface を直接使用[cite: 7]
        THROW_IF_FAILED(baseContext->QueryInterface(IID_PPV_ARGS(m_D2DContext.put())));
    }

    auto Device::CreateBitmap(const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>
    {
        wil::com_ptr<ID2D1Bitmap1> bitmap;
        const auto size       = D2D1::SizeU(static_cast<UINT32>(pixels.Width), static_cast<UINT32>(pixels.Height));
        const auto properties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        THROW_IF_FAILED(m_D2DContext->CreateBitmap(size, pixels.Bytes, pixels.Pitch, properties, bitmap.put()));
        return bitmap;
    }

    auto Device::CreateSolidColorBrush(const D2D1_COLOR_F& color) -> wil::com_ptr<ID2D1SolidColorBrush>
    {
        wil::com_ptr<ID2D1SolidColorBrush> brush;
        THROW_IF_FAILED(m_D2DContext->CreateSolidColorBrush(color, brush.put()));
        return brush;
    }

    auto Device::CreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        wil::com_ptr<IDWriteTextFormat> format;
        THROW_IF_FAILED(m_DWriteFactory->CreateTextFormat(
            fontName.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"ja-jp", format.put()
        ));
        return format;
    }

    auto Device::CreateTextLayout(std::wstring_view text, IDWriteTextFormat* format) -> wil::com_ptr<IDWriteTextLayout>
    {
        wil::com_ptr<IDWriteTextLayout> layout;
        THROW_IF_FAILED(m_DWriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.length()), format, 0.0f, 0.0f, layout.put()));
        return layout;
    }

    auto Device::GetDefaultSpriteBatch() -> ID2D1SpriteBatch&
    {
        // 1. すでに生成されているかチェック
        if (!m_DefaultSpriteBatch)
        {
            // 2. nullptr ならば、その場で生成（遅延初期化）
            // m_D2DContext (ID2D1DeviceContext3) を使用してバッチを作成
            THROW_IF_FAILED(m_D2DContext->CreateSpriteBatch(m_DefaultSpriteBatch.put()));
        }

        return *m_DefaultSpriteBatch.get();
    }

} // namespace N503::Renderer2D::Canvas
