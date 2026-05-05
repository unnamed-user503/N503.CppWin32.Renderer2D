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
            m_Handle = N503CreateSprite(path.data(), { sourceRect.Left, sourceRect.Top, sourceRect.Right, sourceRect.Bottom });
        }

        ~Sprite()
        {
            if (m_Handle)
            {
                N503SpriteDestroy(m_Handle);
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
                    N503SpriteDestroy(m_Handle);
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
                N503SetSpriteTransform(
                    m_Handle,
                    {
                        .Position = { transform.Position.X, transform.Position.Y, },
                        .Rotation = transform.Rotation,
                        .Scale    = { transform.Scale.X, transform.Scale.Y, },
                    }
                );
            }
        }

        auto SetColor(const ColorF color) -> void
        {
            if (m_Handle)
            {
                N503SetSpriteColor(m_Handle, { color.Red, color.Green, color.Blue, color.Alpha });
            }
        }

        auto SetRenderGroup(const RenderGroup group) -> void
        {
            if (m_Handle)
            {
                N503SetSpriteRenderGroup(m_Handle, static_cast<uint32_t>(group));
            }
        }

        auto SetVisible(bool visible) -> void
        {
            if (m_Handle)
            {
                N503SetSpriteVisible(m_Handle, visible ? 1 : 0);
            }
        }

    private:
        N503Sprite m_Handle;
    };

} // namespace N503::Renderer2D
