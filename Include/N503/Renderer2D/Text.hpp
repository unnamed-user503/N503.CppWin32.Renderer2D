#pragma once

// 1. Project Headers
#include <N503/Abi/Api.hpp>
#include <N503/Renderer2D/Details/Api.h>
#include <N503/Renderer2D/Types.hpp>

// 2. Project Dependencies

// 6. C++ Standard Libraries
#include <string_view>
#include <utility> // std::exchange

namespace N503::Renderer2D
{
    class Text final
    {
    public:
        explicit Text(const std::string_view text, const std::string_view font = "ＭＳ ゴシック", const float size = 24.0f, ColorF color = { 1.0f, 1.0f, 1.0f, 1.0f }) : m_Handle(nullptr)
        {
            m_Handle = n503_renderer2d_text_create(text.data(), font.data(), size, color.Red, color.Green, color.Blue, color.Alpha);
        }

        ~Text()
        {
            if (m_Handle)
            {
                n503_renderer2d_text_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        Text(const Text&)                    = delete;

        auto operator=(const Text&) -> Text& = delete;

        Text(Text&& other) noexcept : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(Text&& other) noexcept -> Text&
        {
            if (this != &other)
            {
                if (m_Handle)
                {
                    n503_renderer2d_text_destroy(m_Handle);
                }
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        auto SetTransform(const Geometry::Transform& transform) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_transform(m_Handle, transform.Position.X, transform.Position.Y, transform.Scale.X, transform.Scale.Y, transform.Rotation);
            }
        }

        auto SetColor(const ColorF color) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_color(m_Handle, color.Red, color.Green, color.Blue, color.Alpha);
            }
        }

        auto SetVisible(bool visible) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_visible(m_Handle, visible ? 1 : 0);
            }
        }

        auto SetContent(const std::string_view content) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_content(m_Handle, content.data());
            }
        }

    private:
        n503_renderer2d_text_h m_Handle;
    };

} // namespace N503::Renderer2D
