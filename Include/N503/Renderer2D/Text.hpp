#pragma once

// 1. Project Headers
#include <N503/Abi/Api.hpp>
#include <N503/Renderer2D/Details/Api.h>
#include <N503/Renderer2D/Types.hpp>

// 6. C++ Standard Libraries
#include <string_view>
#include <utility>

namespace N503::Renderer2D
{
    class Text final
    {
    public:
        explicit Text(const std::string_view content, const std::string_view font = "ＭＳ ゴシック", const float size = 24.0f) : m_Handle(nullptr)
        {
            m_Handle = N503CreateText(content.data(), font.data(), size);
        }

        ~Text()
        {
            if (m_Handle)
            {
                N503DestroyText(m_Handle);
                m_Handle = nullptr;
            }
        }

        Text(const Text&) = delete;

        auto operator=(const Text&) -> Text& = delete;

        Text(Text&& other) noexcept : m_Handle(std::exchange(other.m_Handle, nullptr)) {}

        auto operator=(Text&& other) noexcept -> Text&
        {
            if (this != &other)
            {
                if (m_Handle) N503DestroyText(m_Handle);
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        auto SetTransform(const Geometry::Transform2D& transform) -> void
        {
            if (m_Handle)
            {
                N503Transform2D t = {
                    .Position = { transform.Position.X, transform.Position.Y },
                    .Rotation = transform.Rotation,
                    .Scale    = { transform.Scale.X, transform.Scale.Y }
                };
                N503SetTextTransform(m_Handle, t);
            }
        }

        auto SetColor(const ColorF color) -> void
        {
            if (m_Handle)
            {
                N503Color c = { color.Red, color.Green, color.Blue, color.Alpha };
                N503SetTextColor(m_Handle, c);
            }
        }

        auto SetVisible(bool visible) -> void
        {
            if (m_Handle)
            {
                N503SetTextVisible(m_Handle, visible ? 1 : 0);
            }
        }

        auto SetContent(const std::string_view content) -> void
        {
            if (m_Handle)
            {
                N503SetTextContent(m_Handle, content.data());
            }
        }

    private:
        N503Text m_Handle;
    };
}
