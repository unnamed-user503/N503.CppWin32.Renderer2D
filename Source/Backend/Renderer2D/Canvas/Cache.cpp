#include "Pch.hpp"
#include "Cache.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "Device.hpp"
#include "Font/Atlas.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>

// 6. C++ Standard Libraries
#include <format>

namespace N503::Renderer2D::Canvas
{
    namespace
    {
        auto TranscodeUtf8ToWide(std::string_view utf8) -> std::wstring
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
    } // namespace

    Cache::Cache(Device& device) : m_Device(device)
    {
    }

    // =========================================================
    // private キー生成ヘルパー
    // =========================================================

    auto Cache::MakeBrushKey(const ColorF color) -> std::uint32_t
    {
        return (static_cast<uint32_t>(color.Red * 255.0f) << 24) | (static_cast<uint32_t>(color.Green * 255.0f) << 16) |
               (static_cast<uint32_t>(color.Blue * 255.0f) << 8) | static_cast<uint32_t>(color.Alpha * 255.0f);
    }

    auto Cache::MakeTextFormatKey(std::wstring_view fontName, float fontSize) -> std::wstring
    {
        return std::wstring(fontName) + L"_" + std::to_wstring(fontSize);
    }

    auto Cache::MakeTextLayoutKey(std::wstring_view text, IDWriteTextFormat* format) -> std::wstring
    {
        return std::wstring(text) + L"_" + std::to_wstring(reinterpret_cast<uintptr_t>(format));
    }

    // =========================================================
    // ビットマップ
    // =========================================================

    auto Cache::FindBitmap(ResourceHandle handle) -> wil::com_ptr<ID2D1Bitmap1>
    {
        if (auto it = m_Bitmaps.find(handle.ID); it != m_Bitmaps.end())
        {
            return it->second;
        }
        return nullptr;
    }

    auto Cache::StoreBitmap(ResourceHandle handle, wil::com_ptr<ID2D1Bitmap1> bitmap) -> void
    {
#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(
            std::format("[Renderer2D] <Canvas::Cache>: StoreBitmap for ResourceID={}", static_cast<std::uint64_t>(handle.ID))
        );
#endif
        m_Bitmaps[handle.ID] = std::move(bitmap);
    }

    // =========================================================
    // ブラシ
    // =========================================================

    auto Cache::FindBrush(const ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>
    {
        if (auto it = m_Brushes.find(MakeBrushKey(color)); it != m_Brushes.end())
        {
            return it->second;
        }
        return nullptr;
    }

    auto Cache::StoreBrush(const ColorF color, wil::com_ptr<ID2D1SolidColorBrush> brush) -> void
    {
        const auto key = MakeBrushKey(color);
#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format("[Renderer2D] <Canvas::Cache>: StoreBrush for Color=0x{:08X}", key));
#endif
        m_Brushes[key] = std::move(brush);
    }

    // =========================================================
    // テキスト形式
    // =========================================================

    auto Cache::FindTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        if (auto it = m_TextFormats.find(MakeTextFormatKey(fontName, fontSize)); it != m_TextFormats.end())
        {
            return it->second;
        }
        return nullptr;
    }

    auto Cache::StoreTextFormat(std::wstring_view fontName, float fontSize, wil::com_ptr<IDWriteTextFormat> format) -> void
    {
#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(
            std::format(L"[Renderer2D] <Canvas::Cache>: StoreTextFormat for Font={}, Size={}", fontName, fontSize)
        );
#endif
        m_TextFormats[MakeTextFormatKey(fontName, fontSize)] = std::move(format);
    }

    // =========================================================
    // テキストレイアウト
    // =========================================================

    auto Cache::FindTextLayout(std::wstring_view text, IDWriteTextFormat* format) -> wil::com_ptr<IDWriteTextLayout>
    {
        if (auto it = m_TextLayouts.find(MakeTextLayoutKey(text, format)); it != m_TextLayouts.end())
        {
            return it->second;
        }
        return nullptr;
    }

    auto Cache::StoreTextLayout(std::wstring_view text, IDWriteTextFormat* format, wil::com_ptr<IDWriteTextLayout> layout) -> void
    {
#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format(L"[Renderer2D] <Canvas::Cache>: StoreTextLayout for Text length={}", text.length()));
#endif
        m_TextLayouts[MakeTextLayoutKey(text, format)] = std::move(layout);
    }

    // =========================================================
    // フォントアトラス
    // =========================================================

    auto Cache::FindFontAtlas(std::wstring_view familyName, float emSize) -> Font::Atlas*
    {
        const FontAtlasKey key{ std::wstring(familyName), emSize };
        if (auto it = m_FontAtlases.find(key); it != m_FontAtlases.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    auto Cache::StoreFontAtlas(std::wstring_view familyName, float emSize, std::unique_ptr<Font::Atlas> atlas) -> Font::Atlas*
    {
        const FontAtlasKey key{ std::wstring(familyName), emSize };
#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(
            std::format(L"[Renderer2D] <Canvas::Cache>: StoreFontAtlas for Font={}, Size={}", familyName, emSize)
        );
#endif
        auto* raw          = atlas.get();
        m_FontAtlases[key] = std::move(atlas);
        return raw;
    }

    // =========================================================
    // クリア
    // =========================================================

    auto Cache::Clear() -> void
    {
        m_Bitmaps.clear();
        m_Brushes.clear();
        m_TextFormats.clear();
        m_TextLayouts.clear();
        m_FontAtlases.clear();
    }

} // namespace N503::Renderer2D::Canvas
