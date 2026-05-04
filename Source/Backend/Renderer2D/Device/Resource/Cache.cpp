#include "Pch.hpp"
#include "Cache.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "../Context.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <format>

namespace N503::Renderer2D::Device::Resource
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

    Cache::Cache(Device::Context& context) : m_DeviceContext(context)
    {
    }

    auto Cache::Get(ResourceHandle handle) -> wil::com_ptr<ID2D1Bitmap1>
    {
        const auto key = handle.ID;

        if (m_Bitmaps.find(key) == m_Bitmaps.end())
        {
            return nullptr;
        }

        return m_Bitmaps[key];
    }

    auto Cache::GetOrCreateBitmap(ResourceHandle handle, const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>
    {
        const auto key = handle.ID;

        if (m_Bitmaps.find(key) == m_Bitmaps.end())
        {
#ifdef _DEBUG
            Engine::GetInstance().GetDiagnosticsReporter().Verbose(
                std::format("[Renderer2D] <Device::Resource::Cache>: CreateBitmap for ResourceID={}", static_cast<std::uint64_t>(handle.ID))
            );
#endif
            auto bitmap = m_DeviceContext.CreateBitmap(pixels);

            if (!bitmap)
            {
                return nullptr;
            }

            m_Bitmaps[key] = bitmap;
        }

        return m_Bitmaps[key];
    }

    auto Cache::GetOrCreateBrush(const ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>
    {
        // clang-format off
        const uint32_t key = (static_cast<uint32_t>(color.Red   * 255) << 24) |
                             (static_cast<uint32_t>(color.Green * 255) << 16) |
                             (static_cast<uint32_t>(color.Blue  * 255) <<  8) |
                             (static_cast<uint32_t>(color.Alpha * 255));
        // clang-format on

        if (m_Brushes.find(key) == m_Brushes.end())
        {
#ifdef _DEBUG
            Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format("[Renderer2D] <Device::Resource::Cache>: CreateBrush for Color={}", key));
#endif
            auto brush = m_DeviceContext.CreateSolidColorBrush(color);

            if (!brush)
            {
                return nullptr;
            }

            m_Brushes[key] = brush;
        }

        return m_Brushes[key];
    }

    auto Cache::GetOrCreateTextFormat(std::string_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        return GetOrCreateTextFormat(TranscodeUtf8ToWide(fontName), fontSize);
    }

    auto Cache::GetOrCreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>
    {
        const auto key = std::wstring(fontName) + L"_" + std::to_wstring(fontSize);

        if (m_TextFormats.find(key) == m_TextFormats.end())
        {
#ifdef _DEBUG
            Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format(L"[Renderer2D] <Device::Resource::Cache>: CreateTextFormat for Key={}", fontName)
            );
#endif
            auto textFormat = m_DeviceContext.CreateTextFormat(fontName, fontSize);

            if (!textFormat)
            {
                return nullptr;
            }

            m_TextFormats[key] = textFormat;
        }

        return m_TextFormats[key];
    }

    auto Cache::GetOrCreateTextLayout(const std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        return GetOrCreateTextLayout(TranscodeUtf8ToWide(text), textFormat);
    }

    auto Cache::GetOrCreateTextLayout(const std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>
    {
        const auto key = std::wstring(text) + L"_" + std::to_wstring(reinterpret_cast<uintptr_t>(textFormat.get()));

        if (m_TextLayouts.find(key) == m_TextLayouts.end())
        {
#ifdef _DEBUG
            Engine::GetInstance().GetDiagnosticsReporter().Verbose(std::format(L"[Renderer2D] <Device::Resource::Cache>: CreateTextLayout for Key={}", text));
#endif
            auto textLayout = m_DeviceContext.CreateTextLayout(text, textFormat);

            if (!textLayout)
            {
                return nullptr;
            }

            m_TextLayouts[key] = textLayout;
        }

        return m_TextLayouts[key];
    }

} // namespace N503::Renderer2D::Device::Resource
