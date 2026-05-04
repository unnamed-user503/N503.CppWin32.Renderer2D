#pragma once

// 1. Project Headers
#include <N503/Renderer2D/Details/Api.h>
#include <N503/Renderer2D/Types.hpp>

// 2. Project Dependencies

// 6. C++ Standard Libraries
#include <cstdint>
#include <string_view>
#include <utility> // std::exchange

namespace N503::Renderer2D
{
    class Sprite final
    {
    public:
        explicit Sprite(const std::string_view path, const Geometry::RectU sourceRect = {}) : m_Handle(nullptr)
        {
            m_Handle = n503_renderer2d_sprite_create(path.data(), { sourceRect.Left, sourceRect.Top, sourceRect.Right, sourceRect.Bottom });
        }

        ~Sprite()
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        Sprite(const Sprite&) = delete;

        auto operator=(const Sprite&) -> Sprite& = delete;

        Sprite(Sprite&& other) noexcept : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(Sprite&& other) noexcept -> Sprite&
        {
            if (this != &other)
            {
                if (m_Handle)
                {
                    n503_renderer2d_sprite_destroy(m_Handle);
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
                n503_renderer2d_sprite_set_transform(
                    m_Handle,
                    {
                        .position = { transform.Position.X, transform.Position.Y, transform.Position.Z, },
                        .rotation = transform.Rotation,
                        .scale    = { transform.Scale.X, transform.Scale.Y, },
                    }
                );
            }
        }

        auto SetColor(const ColorF color) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_color(m_Handle, { color.Red, color.Green, color.Blue, color.Alpha });
            }
        }

        auto SetRenderGroup(const RenderGroup group) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_render_group(m_Handle, static_cast<uint32_t>(group));
            }
        }

        auto SetVisible(bool visible) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_visible(m_Handle, visible ? 1 : 0);
            }
        }

    private:
        n503_renderer2d_sprite_h m_Handle;
    };

} // namespace N503::Renderer2D
