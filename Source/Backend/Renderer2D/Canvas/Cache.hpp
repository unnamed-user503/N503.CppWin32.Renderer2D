#pragma once

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 5. Windows Headers
#include <d2d1_1.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace N503::Renderer2D::Pixels
{
    struct Buffer;
}

namespace N503::Renderer2D::Canvas
{
    class Device;

    // リソースの再利用を管理するキャッシュクラス[cite: 16]
    class Cache
    {
    public:
        explicit Cache(Device& device);
        ~Cache() = default;

        // コピー禁止
        Cache(const Cache&)                    = delete;
        auto operator=(const Cache&) -> Cache& = delete;

        // ---- ビットマップ管理 ----
        auto Get(ResourceHandle handle) -> wil::com_ptr<ID2D1Bitmap1>;
        auto GetOrCreateBitmap(ResourceHandle handle, const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>;

        // ---- ブラシ管理 (ColorFをキーとする)[cite: 15] ----
        auto GetOrCreateBrush(const ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>;

        // ---- テキスト形式管理 ----
        auto GetOrCreateTextFormat(std::string_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;
        auto GetOrCreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;

        // ---- テキストレイアウト管理 ----
        auto GetOrCreateTextLayout(std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;
        auto GetOrCreateTextLayout(std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;

        // キャッシュのクリア
        auto Clear() -> void;

    private:
        Device& m_Device; // 命名規則: m_PascalCase[cite: 16]

        std::unordered_map<Handle::ResourceID, wil::com_ptr<ID2D1Bitmap1>> m_Bitmaps;
        std::unordered_map<std::uint32_t, wil::com_ptr<ID2D1SolidColorBrush>> m_Brushes;
        std::unordered_map<std::wstring, wil::com_ptr<IDWriteTextFormat>> m_TextFormats;
        std::unordered_map<std::wstring, wil::com_ptr<IDWriteTextLayout>> m_TextLayouts;
    };

} // namespace N503::Renderer2D::Canvas
