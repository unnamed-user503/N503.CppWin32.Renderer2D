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
#include <memory>
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
    class FontAtlas;
}

namespace N503::Renderer2D::Canvas
{
    // リソースの保管・検索のみを担うキャッシュクラス
    // 生成ロジックは持たない。全ての GetOrCreate フローは Device が制御する。
    class Cache
    {
    public:
        explicit Cache(Device& device);
        ~Cache() = default;

        // コピー禁止
        Cache(const Cache&)                    = delete;
        auto operator=(const Cache&) -> Cache& = delete;

        // ---- ビットマップ ----
        auto FindBitmap(ResourceHandle handle) -> wil::com_ptr<ID2D1Bitmap1>;
        auto StoreBitmap(ResourceHandle handle, wil::com_ptr<ID2D1Bitmap1> bitmap) -> void;

        // ---- ブラシ ----
        auto FindBrush(ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>;
        auto StoreBrush(ColorF color, wil::com_ptr<ID2D1SolidColorBrush> brush) -> void;

        // ---- テキスト形式 ----
        auto FindTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;
        auto StoreTextFormat(std::wstring_view fontName, float fontSize, wil::com_ptr<IDWriteTextFormat> format) -> void;

        // ---- テキストレイアウト ----
        auto FindTextLayout(std::wstring_view text, IDWriteTextFormat* format) -> wil::com_ptr<IDWriteTextLayout>;
        auto StoreTextLayout(std::wstring_view text, IDWriteTextFormat* format, wil::com_ptr<IDWriteTextLayout> layout) -> void;

        // ---- フォントアトラス ----
        auto FindFontAtlas(std::wstring_view familyName, float emSize) -> FontAtlas*;
        auto StoreFontAtlas(std::wstring_view familyName, float emSize, std::unique_ptr<FontAtlas> atlas) -> FontAtlas*;

        // ---- キャッシュのクリア ----
        auto Clear() -> void;

    private:
        static auto MakeTextLayoutKey(std::wstring_view text, IDWriteTextFormat* format) -> std::wstring;
        static auto MakeTextFormatKey(std::wstring_view fontName, float fontSize) -> std::wstring;
        static auto MakeBrushKey(ColorF color) -> std::uint32_t;

        struct FontAtlasKey
        {
            std::wstring FamilyName;
            float        EmSize;

            auto operator==(const FontAtlasKey&) const -> bool = default;
        };

        struct FontAtlasKeyHash
        {
            auto operator()(const FontAtlasKey& k) const -> size_t
            {
                size_t h = std::hash<std::wstring>{}(k.FamilyName);
                h ^= std::hash<float>{}(k.EmSize) + 0x9e3779b9 + (h << 6) + (h >> 2);
                return h;
            }
        };

    private:
        Device& m_Device;

        std::unordered_map<Handle::ResourceID, wil::com_ptr<ID2D1Bitmap1>>            m_Bitmaps;
        std::unordered_map<std::uint32_t, wil::com_ptr<ID2D1SolidColorBrush>>         m_Brushes;
        std::unordered_map<std::wstring, wil::com_ptr<IDWriteTextFormat>>              m_TextFormats;
        std::unordered_map<std::wstring, wil::com_ptr<IDWriteTextLayout>>              m_TextLayouts;
        std::unordered_map<FontAtlasKey, std::unique_ptr<FontAtlas>, FontAtlasKeyHash> m_FontAtlases;
    };

} // namespace N503::Renderer2D::Canvas
