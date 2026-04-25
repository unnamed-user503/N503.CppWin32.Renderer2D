#include "Pch.hpp"
#include "DrawTextLayout.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Resource/Container.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/result.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>

// 6. C++ Standard Libraries
#include <cfloat>
#include <limits>
#include <string>
#include <string_view>

namespace N503::Renderer2D::Device::Commands
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

    auto DrawTextLayout::Execute(Context& context) -> void
    {
        if (!Format)
        {
            auto hr = context.GetDWriteFactory()->CreateTextFormat(
                TranscodeUtf8ToWide(Font.Family).c_str(),
                nullptr,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                Font.Size,
                L"ja-jp", // ロケール
                Format.put()
            );

            THROW_IF_FAILED(hr);
        }

        if (!Brush)
        {
            auto hr = context.GetD2DContext()->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White),
                Brush.put()
            );
            THROW_IF_FAILED(hr);
        }

        if (!Layout)
        {
            auto dwriteFactory = context.GetDWriteFactory();
            auto hr = dwriteFactory->CreateTextLayout(
                TranscodeUtf8ToWide(Text).c_str(),
                static_cast<UINT32>(Text.length()),
                Format.get(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                &Layout
            );
            THROW_IF_FAILED(hr);
        }

        context.GetD2DContext()->DrawTextLayout(
            D2D1::Point2F(Destination.X, Destination.Y),
            Layout.get(),
            Brush.get());
    }

} // namespace N503::Renderer2D::Device::Commands
