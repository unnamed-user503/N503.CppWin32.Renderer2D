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
            m_Handle = N503CreateSpriteGroup(path.data(), static_cast<uint32_t>(count), { sourceRect.Left, sourceRect.Top, sourceRect.Right, sourceRect.Bottom });
        }

        ~SpriteGroup()
        {
            if (m_Handle)
            {
                N503DestroySpriteGroup(m_Handle);
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
                    N503DestroySpriteGroup(m_Handle);
                }
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        auto SetTransformBatch(std::function<bool(const std::uint64_t, Geometry::Transform&)> delegate) -> void
        {
            if (!m_Handle || !delegate)
            {
                return;
            }

            N503SetSpriteGroupTransformBatch(
                m_Handle,
                [](uint64_t index, N503Transform2D* out_transform, void* userData) -> uint32_t
                {
                    auto& callback = *static_cast<std::function<bool(const std::uint64_t, Geometry::Transform&)>*>(userData);

                    Geometry::Transform transform;
                    if (callback(index, transform))
                    {
                        out_transform->Position.X = transform.Position.X;
                        out_transform->Position.Y = transform.Position.Y;
                        out_transform->Rotation   = transform.Rotation;
                        out_transform->Scale.X    = transform.Scale.X;
                        out_transform->Scale.Y    = transform.Scale.Y;
                        return 1; // 変更あり
                    }

                    return 0; // 変更なし
                },
                &delegate
            );
        }

        auto SetColorBatch(std::function<bool(const std::uint64_t, ColorF&)> delegate) -> void
        {
            if (!m_Handle || !delegate)
            {
                return;
            }

            N503SetSpriteGroupColorBatch(
                m_Handle,
                [](uint64_t index, N503Color* out_color, void* userData) -> uint32_t
                {
                    auto& func = *static_cast<std::function<bool(const std::uint64_t, ColorF&)>*>(userData);

                    ColorF c;
                    if (func(index, c))
                    {
                        out_color->Red   = c.Red;
                        out_color->Green = c.Green;
                        out_color->Blue  = c.Blue;
                        out_color->Alpha = c.Alpha;
                        return 1;
                    }
                    return 0;
                },
                &delegate
            );
        }

        // 一括可視性設定（Batch）[cite: 3]
        /*
        auto SetVisibleBatch(std::function<bool(const std::uint64_t, bool&)> delegate) -> void
        {
            if (!m_Handle || !delegate)
            {
                return;
            }

            N503SetSpriteGroupVisibleBatch(
                m_Handle,
                [](uint64_t index, uint32_t visible_val, void* userData) -> uint32_t
                {
                    auto& func = *static_cast<std::function<bool(const std::uint64_t, bool&)>*>(userData);
                    
                    bool visible;
                    if (func(index, visible))
                    {
                        // 内部的な可視性フラグの更新ロジック
                        return visible ? 1 : 0;
                    }
                    return visible_val; // 変更なしの場合は現在の状態を維持
                },
                &delegate
            );
        }*/

        auto SetRenderGroupBatch(const RenderGroup group) -> void
        {
            if (m_Handle)
            {
                return;
            }

            N503SetSpriteGroupRenderGroupBatch(m_Handle, static_cast<uint32_t>(group));
        }

    private:
        N503SpriteGroup m_Handle;
    };

} // namespace N503::Renderer2D
