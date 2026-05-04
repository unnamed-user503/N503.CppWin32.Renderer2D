#include "Pch.hpp"
#include "Device.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "../Pixels/Buffer.hpp"
#include "Cache.hpp"
#include "Font/Atlas.hpp"
#include "Surface.hpp"

// 3. WIL
#include <wil/result_macros.h>

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <format>

namespace N503::Renderer2D::Canvas
{
    Device::Device()
    {
        InitializeDirectX();
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
        THROW_IF_FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), m_DWriteFactory.put_unknown()));

        // 2. Direct3D11 デバイスの作成
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

        // 3. DXGI デバイス経由で Direct2D デバイスを作成
        auto dxgiDevice = m_D3DDevice.query<IDXGIDevice>();
        THROW_IF_FAILED(m_D2DFactory->CreateDevice(dxgiDevice.get(), m_D2DDevice.put()));

        // 4. デバイスコンテキストの作成とアップグレード
        wil::com_ptr<ID2D1DeviceContext> baseContext;
        THROW_IF_FAILED(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, baseContext.put()));
        THROW_IF_FAILED(baseContext->QueryInterface(IID_PPV_ARGS(m_D2DContext.put())));
    }

    // =========================================================
    // キャッシュ統合リソース取得
    // フロー: Cache::Find → (miss時) Create → Cache::Store → return
    // =========================================================

    auto Device::GetOrCreateBitmap(ResourceHandle handle, const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>
    {
        if (auto cached = m_ResourceCache->FindBitmap(handle))
        {
            return cached;
        }

        auto bitmap = CreateBitmap(pixels);
        m_ResourceCache->StoreBitmap(handle, bitmap);
        return bitmap;
    }

    auto Device::GetOrCreateBrush(const ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>
    {
        if (auto cached = m_ResourceCache->FindBrush(color))
        {
            return cached;
        }

        auto brush = CreateSolidColorBrush(D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha));
        m_ResourceCache->StoreBrush(color, brush);
        return brush;
    }

    auto Device::GetOrCreateTextFormat(std::string_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        return GetOrCreateTextFormat(TranscodeUtf8ToWide(fontName), fontSize);
    }

    auto Device::GetOrCreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        if (auto cached = m_ResourceCache->FindTextFormat(fontName, fontSize))
        {
            return cached;
        }

        auto format = CreateTextFormat(fontName, fontSize);
        m_ResourceCache->StoreTextFormat(fontName, fontSize, format);
        return format;
    }

    auto Device::GetOrCreateTextLayout(std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        return GetOrCreateTextLayout(TranscodeUtf8ToWide(text), std::move(textFormat));
    }

    auto Device::GetOrCreateTextLayout(std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        if (auto cached = m_ResourceCache->FindTextLayout(text, textFormat.get()))
        {
            return cached;
        }

        auto layout = CreateTextLayout(text, textFormat.get());
        m_ResourceCache->StoreTextLayout(text, textFormat.get(), layout);
        return layout;
    }

    auto Device::GetOrCreateFontAtlas(std::wstring_view familyName, float emSize) -> Font::Atlas*
    {
        if (auto* cached = m_ResourceCache->FindFontAtlas(familyName, emSize))
        {
            return cached;
        }

        auto atlas = CreateFontAtlas(familyName, emSize);
        return m_ResourceCache->StoreFontAtlas(familyName, emSize, std::move(atlas));
    }

    auto Device::GetDefaultSpriteBatch() -> ID2D1SpriteBatch&
    {
        if (!m_DefaultSpriteBatch)
        {
            THROW_IF_FAILED(m_D2DContext->CreateSpriteBatch(m_DefaultSpriteBatch.put()));
        }
        return *m_DefaultSpriteBatch.get();
    }

    // =========================================================
    // 実生成メソッド (Device 内部からのみ呼ばれる)
    // =========================================================

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

    auto Device::CreateFontAtlas(std::wstring_view familyName, float emSize) -> std::unique_ptr<Font::Atlas>
    {
        // 1. システムフォントコレクションを取得
        wil::com_ptr<IDWriteFontCollection> collection;
        m_DWriteFactory->GetSystemFontCollection(&collection, FALSE);

        // 2. ファミリー名からインデックスを検索
        UINT32 familyIndex{};
        BOOL exists{};
        collection->FindFamilyName(familyName.data(), &familyIndex, &exists);

        // ★ 見つからなければフォールバック
        if (!exists)
        {
            constexpr std::wstring_view FallbackFont = L"Yu Gothic UI";
            collection->FindFamilyName(FallbackFont.data(), &familyIndex, &exists);

#ifdef _DEBUG
            Engine::GetInstance().GetDiagnosticsReporter().Verbose(
                std::format(L"[Renderer2D] FontAtlas: '{}' not found, falling back to '{}'", familyName, FallbackFont)
            );
#endif

            if (!exists)
            {
                return nullptr; // フォールバックも見つからない場合のみ nullptr
            }
        }

        // 3. FontFamily → Font → FontFace
        wil::com_ptr<IDWriteFontFamily> fontFamily;
        collection->GetFontFamily(familyIndex, &fontFamily);

        wil::com_ptr<IDWriteFont> font;
        fontFamily->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL, &font);

        wil::com_ptr<IDWriteFontFace> fontFaceBase;
        font->CreateFontFace(&fontFaceBase);

        wil::com_ptr<IDWriteFontFace3> fontFace;
        fontFaceBase.query_to(&fontFace);

        // 4. FontAtlas を生成して返す
        return std::make_unique<Font::Atlas>(m_D2DContext.get(), m_DWriteFactory.get(), fontFace.get(), emSize, Font::Atlas::BasicLatin);
    }

    auto Device::TranscodeUtf8ToWide(std::string_view utf8) -> std::wstring
    {
        if (utf8.empty())
        {
            return {};
        }
        int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.length()), nullptr, 0);
        if (desired == 0)
        {
            return {};
        }
        std::wstring result(desired, 0);
        ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.length()), &result[0], desired);
        return result;
    }
#ifdef _DEBUG
    auto Device::DumpInstalledFonts() -> void
    {
        wil::com_ptr<IDWriteFontCollection> collection;
        m_DWriteFactory->GetSystemFontCollection(&collection, FALSE);

        const UINT32 count = collection->GetFontFamilyCount();
        for (UINT32 i = 0; i < count; ++i)
        {
            wil::com_ptr<IDWriteFontFamily> family;
            collection->GetFontFamily(i, &family);

            wil::com_ptr<IDWriteLocalizedStrings> names;
            family->GetFamilyNames(&names);

            UINT32 index{};
            BOOL found{};
            names->FindLocaleName(L"ja-jp", &index, &found);
            if (!found)
            {
                names->FindLocaleName(L"en-us", &index, &found);
            }
            if (!found)
            {
                index = 0;
            }

            wchar_t buf[256]{};
            names->GetString(index, buf, 256);

            OutputDebugStringW((std::wstring(buf) + L"\n").c_str());
        }
    }
#endif
} // namespace N503::Renderer2D::Canvas
