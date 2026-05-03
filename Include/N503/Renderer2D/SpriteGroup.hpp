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
        /// @brief 大量体（SpriteGroup）を生成します。
        explicit SpriteGroup(const std::string_view path, std::size_t count, const Geometry::RectU sourceRect = {})
            : m_Handle(nullptr)
        {
            m_Handle = n503_renderer2d_sprite_group_create(
                path.data(), 
                static_cast<uint32_t>(count),
                sourceRect.Left, sourceRect.Top, sourceRect.Right, sourceRect.Bottom
            );
        }

        ~SpriteGroup()
        {
            if (m_Handle)
            {
                // C-API 側に destroy が定義されている前提
                // もし共通の destroy があればそれを使用
                n503_renderer2d_sprite_group_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        // コピー禁止
        SpriteGroup(const SpriteGroup&) = delete;
        auto operator=(const SpriteGroup&) -> SpriteGroup& = delete;

        // ムーブ許可
        SpriteGroup(SpriteGroup&& other) noexcept
            : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(SpriteGroup&& other) noexcept -> SpriteGroup&
        {
            if (this != &other)
            {
                if (m_Handle) n503_renderer2d_sprite_group_destroy(m_Handle);
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        /// @brief 各要素の Transform をデリゲート経由で一括更新します。
        auto SetTransform(std::function<bool(const std::uint64_t, Geometry::Transform&)> delegate) -> void
        {
            if (!m_Handle || !delegate) return;

            // C-API の関数ポインタ型に適合するラムダを渡す
            // delegate はこのスコープ内で即座に実行（Invoke）されるため、キャプチャしても安全
            n503_renderer2d_sprite_group_set_transform(m_Handle, 
                [](uint64_t index, float* x, float* y, float* sx, float* sy, float* rot, void* userData) -> uint32_t {
                    auto& func = *static_cast<std::function<bool(const std::uint64_t, Geometry::Transform&)>*>(userData);
                    
                    Geometry::Transform t;
                    // 必要に応じて現在の値を t に詰める（現在は新規作成を想定）
                    if (func(index, t)) {
                        *x = t.Position.X; *y = t.Position.Y;
                        *sx = t.Scale.X;  *sy = t.Scale.Y;
                        *rot = t.Rotation;
                        return 1; // Dirty
                    }
                    return 0; // Clean
                }, 
                &delegate // userData として delegate 本体のポインタを渡す
            );
        }

        /// @brief 各要素の色をデリゲート経由で一括更新します。
        auto SetColor(std::function<bool(const std::uint64_t, ColorF&)> delegate) -> void
        {
            if (!m_Handle || !delegate) return;

            n503_renderer2d_sprite_group_set_color(m_Handle,
                [](uint64_t index, float* r, float* g, float* b, float* a, void* userData) -> uint32_t {
                    auto& func = *static_cast<std::function<bool(const std::uint64_t, ColorF&)>*>(userData);
                    
                    ColorF c;
                    if (func(index, c)) {
                        *r = c.Red; *g = c.Green; *b = c.Blue; *a = c.Alpha;
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
