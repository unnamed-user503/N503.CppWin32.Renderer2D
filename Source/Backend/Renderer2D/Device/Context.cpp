#include "Pch.hpp"
#include "Context.hpp"

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "RenderTarget.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <memory>
#include <string>
#include <string_view>

namespace N503::Renderer2D::Device
{

    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    Context::Context()
    {
        // D2D1FactoryとDWriteFactoryの作成
        D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        THROW_IF_FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, m_D2DFactory.put_void()));
        THROW_IF_FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), m_DWriteFactory.put_unknown()));

        // Direct3D11のリソースを作成する
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

        wil::com_ptr<ID3D11Device> d3d11Device;
        wil::com_ptr<ID3D11DeviceContext> d3d11Context;

        THROW_IF_FAILED(::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            d3d11Device.put(),
            nullptr,
            d3d11Context.put()
        ));

        m_D3DDevice  = std::move(d3d11Device);
        m_D3DContext = std::move(d3d11Context);

        // Direct3D11のリソースからDirect2Dのリソースを作成する
        auto dxgiDevice = m_D3DDevice.query<IDXGIDevice>();

        THROW_IF_FAILED(m_D2DFactory->CreateDevice(dxgiDevice.get(), m_D2DDevice.put()));
        THROW_IF_FAILED(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_D2DContext.put()));

        // SpriteBatchの作成
        THROW_IF_FAILED(m_D2DContext->QueryInterface(IID_PPV_ARGS(&m_D2DContext3)));
        THROW_IF_FAILED(m_D2DContext3->CreateSpriteBatch(&m_SpriteBatch));
    }

    auto Context::CreateBitmap(const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>
    {
        wil::com_ptr<ID2D1Bitmap1> bitmap;

        auto hr = m_D2DContext->CreateBitmap(
            D2D1::SizeU(static_cast<UINT32>(pixels.Width), static_cast<UINT32>(pixels.Height)),
            pixels.Bytes,
            pixels.Pitch,
            D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
            bitmap.put()
        );

        if (FAILED(hr))
        {
            return nullptr;
        }

        return bitmap;
    }

    auto Context::CreateSolidColorBrush(const ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>
    {
        wil::com_ptr<ID2D1SolidColorBrush> solidBrush;

        const auto d2dColor = D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha);

        auto hr = m_D2DContext->CreateSolidColorBrush(d2dColor, solidBrush.put());

        if (FAILED(hr))
        {
            return nullptr;
        }

        return solidBrush;
    }

    auto Context::CreateTextFormat(const std::string_view fontName, const float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        return CreateTextFormat(TranscodeUtf8ToWide(fontName), fontSize);
    }

    auto Context::CreateTextFormat(const std::wstring_view fontName, const float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        wil::com_ptr<IDWriteTextFormat> textFormat;

        auto hr = m_DWriteFactory->CreateTextFormat(
            fontName.data(),
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            fontSize,
            L"ja-jp",
            textFormat.put()
        );

        if (FAILED(hr))
        {
            return nullptr;
        }

        return textFormat;
    }

    auto Context::CreateTextLayout(const std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        return CreateTextLayout(TranscodeUtf8ToWide(text), textFormat);
    }

    auto Context::CreateTextLayout(const std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        wil::com_ptr<IDWriteTextLayout> textLayout;

        auto hr = m_DWriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.length()), textFormat.get(), FLT_MAX, FLT_MAX, textLayout.put());

        if (FAILED(hr))
        {
            return nullptr;
        }

        return textLayout;
    }

    auto Device::Context::DrawBitmap(wil::com_ptr<ID2D1Bitmap1> bitmap, const D2D1_RECT_F destination) -> void
    {
        m_D2DContext->DrawBitmap(bitmap.get(), destination);
    }

    auto Device::Context::DrawTextLayout(const Geometry::Point2F origin, wil::com_ptr<IDWriteTextLayout> textLayout, wil::com_ptr<ID2D1SolidColorBrush> brush)
        -> void
    {
        m_D2DContext->DrawTextLayout(D2D1::Point2F(origin.X, origin.Y), textLayout.get(), brush.get());
    }

    auto Device::Context::SetTransform(const D2D1_MATRIX_3X2_F& transform) -> void
    {
        m_D2DContext->SetTransform(transform);
    }

    auto Context::BeginDraw(const ColorF clearColor) -> bool
    {
        if (!m_D2DContext)
        {
            return false;
        }

        m_D2DContext->BeginDraw();
        // m_D2DContext->SetDpi(96.0f, 96.0f);
        m_D2DContext->SetUnitMode(D2D1_UNIT_MODE_PIXELS);
        m_D2DContext->Clear(D2D1::ColorF(clearColor.Red, clearColor.Green, clearColor.Blue, clearColor.Alpha));

        return true;
    }

    auto Context::EndDraw() -> HRESULT
    {
        // D2D の描画命令を確定させる
        HRESULT hr = m_D2DContext->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            // デバイスロスト発生。外部に通知してコンテキストを再構築させる
        }

        return hr;
    }

    auto Context::Reset() -> void
    {
        m_D2DContext.reset();
        m_D2DDevice.reset();
        m_D3DContext.reset();
        m_D3DDevice.reset();
    }

    auto Device::Context::SetRenderTarget(RenderTarget* renderTarget) -> void
    {
        m_D2DContext->SetTarget(renderTarget->GetTargetBitmap().get());
    }

} // namespace N503::Renderer2D::Device
