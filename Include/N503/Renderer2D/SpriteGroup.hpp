#pragma once

// 1. Project Headers
#include <N503/Abi/Api.hpp>
#include <N503/Renderer2D/Details/Api.h>
#include <N503/Renderer2D/Types.hpp>

// 2. Project Dependencies

// 6. C++ Standard Libraries
#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>

namespace N503::Renderer2D
{
    class SpriteGroup final
    {
    public:
        explicit SpriteGroup(const std::string_view path, std::size_t count, const Geometry::RectU sourceRect = {}) : m_Handle(nullptr)
        {
            m_Handle = n503_renderer2d_sprite_group_create(
                path.data(), static_cast<uint32_t>(count), sourceRect.Left, sourceRect.Top, sourceRect.Right, sourceRect.Bottom
            );
        }

        ~SpriteGroup()
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_group_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        SpriteGroup(const SpriteGroup&) = delete;

        auto operator=(const SpriteGroup&) -> SpriteGroup& = delete;

        SpriteGroup(SpriteGroup&& other) noexcept : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(SpriteGroup&& other) noexcept -> SpriteGroup&
        {
            if (this != &other)
            {
                if (m_Handle)
                {
                    n503_renderer2d_sprite_group_destroy(m_Handle);
                }
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        auto SetTransform(std::function<bool(const std::uint64_t, Geometry::Transform&)> delegate) -> void
        {
            if (!m_Handle || !delegate)
            {
                return;
            }

            n503_renderer2d_sprite_group_set_transform(
                m_Handle,
                [](uint64_t index, n503_transform_t* out_transform, void* userData) -> uint32_t
                {
                    auto& callback = *static_cast<std::function<bool(const std::uint64_t, Geometry::Transform&)>*>(userData);

                    Geometry::Transform transform;
                    if (callback(index, transform))
                    {
                        out_transform->position.x = transform.Position.X;
                        out_transform->position.y = transform.Position.Y;
                        out_transform->rotation   = transform.Rotation;
                        out_transform->scale.x    = transform.Scale.X;
                        out_transform->scale.y    = transform.Scale.Y;
                        return 1; // Dirty
                    }

                    return 0; // Clean
                },
                &delegate
            );
        }

        auto SetColor(std::function<bool(const std::uint64_t, ColorF&)> delegate) -> void
        {
            if (!m_Handle || !delegate)
            {
                return;
            }

            n503_renderer2d_sprite_group_set_color(
                m_Handle,
                [](uint64_t index, n503_color_t* out_color, void* userData) -> uint32_t
                {
                    auto& func = *static_cast<std::function<bool(const std::uint64_t, ColorF&)>*>(userData);

                    ColorF c;
                    if (func(index, c))
                    {
                        out_color->red   = c.Red;
                        out_color->green = c.Green;
                        out_color->blue  = c.Blue;
                        out_color->alpha = c.Alpha;
                        return 1;
                    }
                    return 0;
                },
                &delegate
            );
        }

    private:
        n503_renderer2d_sprite_group_h m_Handle;
    };

} // namespace N503::Renderer2D
