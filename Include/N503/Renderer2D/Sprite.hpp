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
        /// @brief 指定されたテクスチャパスと矩形領域からスプライトを生成します。
        explicit Sprite(const std::string_view path, const Geometry::RectU sourceRect = {})
            : m_Handle(nullptr)
        {
            // C-API を呼び出し、構造体をバラして渡す
            m_Handle = n503_renderer2d_sprite_create(
                path.data(), 
                sourceRect.Left, sourceRect.Top, sourceRect.Right, sourceRect.Bottom
            );
        }

        /// @brief リソースを解放します。
        ~Sprite()
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        // コピー禁止
        Sprite(const Sprite&) = delete;
        auto operator=(const Sprite&) -> Sprite& = delete;

        // ムーブ許可
        Sprite(Sprite&& other) noexcept
            : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(Sprite&& other) noexcept -> Sprite&
        {
            if (this != &other)
            {
                if (m_Handle) n503_renderer2d_sprite_destroy(m_Handle);
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        /// @brief 位置、回転、スケールを一括で設定します。
        auto SetTransform(const Geometry::Transform& transform) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_transform(
                    m_Handle, 
                    transform.Position.X, transform.Position.Y, 
                    transform.Scale.X, transform.Scale.Y, 
                    transform.Rotation
                );
            }
        }

        /// @brief 色（RGBA）を設定します。
        auto SetColor(const ColorF color) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_color(m_Handle, color.Red, color.Green, color.Blue, color.Alpha);
            }
        }

        /// @brief 描画グループ（優先順位）を設定します。
        auto SetRenderGroup(const RenderGroup group) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_render_group(m_Handle, static_cast<uint32_t>(group));
            }
        }

        /// @brief 表示・非表示を切り替えます。
        auto SetVisible(bool visible) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_sprite_set_visible(m_Handle, visible ? 1 : 0);
            }
        }

    private:
        /// @brief DLL 内部で管理される実体へのハンドル
        n503_renderer2d_sprite_h m_Handle;
    };

} // namespace N503::Renderer2D
