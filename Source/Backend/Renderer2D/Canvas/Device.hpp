#pragma once

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 5. Windows Headers
#include <d2d1_3.h>
#include <d3d11.h>
#include <dwrite_3.h>

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>

namespace N503::Renderer2D::Pixels
{
    struct Buffer;
}

namespace N503::Renderer2D::Canvas::Font
{
    class Atlas;
}

namespace N503::Renderer2D::Canvas
{
    class Cache;
}

namespace N503::Renderer2D::Canvas
{

    class Device
    {
    public:
        Device();
        ~Device();

        // コピー禁止
        Device(const Device&)                    = delete;
        auto operator=(const Device&) -> Device& = delete;

        // ---- アクセッサ ----
        auto GetD3D11Device() const noexcept -> ID3D11Device&
        {
            return *m_D3DDevice.get();
        }
        auto GetD2D1DeviceContext() const noexcept -> ID2D1DeviceContext3&
        {
            return *m_D2DContext.get();
        }
        auto GetCache() noexcept -> Canvas::Cache&
        {
            return *m_ResourceCache;
        }

        // ---- キャッシュ統合リソース取得 (呼び出し側が使う唯一のAPI) ----
        // フロー: Cache::Find → (miss時) Create → Cache::Store → return
        auto GetOrCreateBitmap(ResourceHandle handle, const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>;
        auto GetOrCreateBrush(ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>;
        auto GetOrCreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;
        auto GetOrCreateTextFormat(std::string_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;
        auto GetOrCreateTextLayout(std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;
        auto GetOrCreateTextLayout(std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;
        auto GetOrCreateFontAtlas(std::wstring_view familyName, float emSize) -> Font::Atlas*;

        // ---- SpriteBatch (キャッシュ対象外・単一インスタンス) ----
        auto GetDefaultSpriteBatch() -> ID2D1SpriteBatch&;

    private:
        auto InitializeDirectX() -> void;

        // ---- 実生成メソッド (Device 内部からのみ呼ばれる) ----
        auto CreateBitmap(const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>;
        auto CreateSolidColorBrush(const D2D1_COLOR_F& color) -> wil::com_ptr<ID2D1SolidColorBrush>;
        auto CreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;
        auto CreateTextLayout(std::wstring_view text, IDWriteTextFormat* format) -> wil::com_ptr<IDWriteTextLayout>;
        auto CreateFontAtlas(std::wstring_view familyName, float emSize) -> std::unique_ptr<Font::Atlas>;

        auto TranscodeUtf8ToWide(std::string_view utf8) -> std::wstring;

        auto DumpInstalledFonts() -> void;

    private:
        // 命名規則: m_PascalCase
        wil::com_ptr<ID3D11Device> m_D3DDevice;
        wil::com_ptr<ID3D11DeviceContext> m_D3DContext;

        wil::com_ptr<ID2D1Factory3> m_D2DFactory;
        wil::com_ptr<ID2D1Device2> m_D2DDevice;
        wil::com_ptr<ID2D1DeviceContext3> m_D2DContext;

        wil::com_ptr<IDWriteFactory3> m_DWriteFactory;

        wil::com_ptr<ID2D1SpriteBatch> m_DefaultSpriteBatch;

        std::unique_ptr<Canvas::Cache> m_ResourceCache;
    };

} // namespace N503::Renderer2D::Canvas
