#include "Pch.hpp"
#include "Cache.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "Device.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>

// 6. C++ Standard Libraries
#include <format>

namespace N503::Renderer2D::Canvas
{
    namespace
    {
        // UTF-8 から ワイド文字列への変換ユーティリティ[cite: 15]
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
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

    auto Cache::Get(ResourceHandle handle) -> wil::com_ptr<ID2D1Bitmap1>
    {
        const auto key = handle.ID;
        if (auto it = m_Bitmaps.find(key); it != m_Bitmaps.end())
        {
            return it->second;
        }
        return nullptr;
    }

    auto Cache::GetOrCreateBitmap(ResourceHandle handle, const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>
    {
        const auto key = handle.ID;
        if (auto it = m_Bitmaps.find(key); it != m_Bitmaps.end())
        {
            return it->second;
        }

#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(
            std::format("[Renderer2D] <Canvas::Cache>: CreateBitmap for ResourceID={}", static_cast<std::uint64_t>(handle.ID))
        );
#endif

        auto bitmap = m_Device.CreateBitmap(pixels);
        if (bitmap)
        {
            m_Bitmaps[key] = bitmap;
        }
        return bitmap;
    }

    auto Cache::GetOrCreateBrush(const ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>
    {
        // RGBAをパックして32bitキーを生成[cite: 15]
        const uint32_t key = (static_cast<uint32_t>(color.Red * 255.0f) << 24) | (static_cast<uint32_t>(color.Green * 255.0f) << 16) |
                             (static_cast<uint32_t>(color.Blue * 255.0f) << 8) | (static_cast<uint32_t>(color.Alpha * 255.0f));

        if (auto it = m_Brushes.find(key); it != m_Brushes.end())
        {
            return it->second;
        }

#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format("[Renderer2D] <Canvas::Cache>: CreateBrush for Color=0x{:08X}", key));
#endif

        auto brush = m_Device.CreateSolidColorBrush(D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha));
        if (brush)
        {
            m_Brushes[key] = brush;
        }
        return brush;
    }

    auto Cache::GetOrCreateTextFormat(std::string_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        return GetOrCreateTextFormat(TranscodeUtf8ToWide(fontName), fontSize);
    }

    auto Cache::GetOrCreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        const std::wstring key = std::wstring(fontName) + L"_" + std::to_wstring(fontSize);

        if (auto it = m_TextFormats.find(key); it != m_TextFormats.end())
        {
            return it->second;
        }

#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(
            std::format(L"[Renderer2D] <Canvas::Cache>: CreateTextFormat for Font={}, Size={}", fontName, fontSize)
        );
#endif

        auto textFormat = m_Device.CreateTextFormat(fontName, fontSize);
        if (textFormat)
        {
            m_TextFormats[key] = textFormat;
        }
        return textFormat;
    }

    auto Cache::GetOrCreateTextLayout(const std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        return GetOrCreateTextLayout(TranscodeUtf8ToWide(text), textFormat);
    }

    auto Cache::GetOrCreateTextLayout(const std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        // テキスト内容とフォーマットのポインタを組み合わせてキーを生成[cite: 15]
        const std::wstring key = std::wstring(text) + L"_" + std::to_wstring(reinterpret_cast<uintptr_t>(textFormat.get()));

        if (auto it = m_TextLayouts.find(key); it != m_TextLayouts.end())
        {
            return it->second;
        }

#ifdef _DEBUG
        Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format(L"[Renderer2D] <Canvas::Cache>: CreateTextLayout for Text length={}", text.length())
        );
#endif

        auto textLayout = m_Device.CreateTextLayout(text, textFormat.get());
        if (textLayout)
        {
            m_TextLayouts[key] = textLayout;
        }
        return textLayout;
    }

    auto Cache::Clear() -> void
    {
        m_Bitmaps.clear();
        m_Brushes.clear();
        m_TextFormats.clear();
        m_TextLayouts.clear();
    }

} // namespace N503::Renderer2D::Canvas
