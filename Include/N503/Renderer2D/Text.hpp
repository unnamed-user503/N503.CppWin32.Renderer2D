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
        /// @brief テキストオブジェクトを生成します。
        explicit Text(
            const std::string_view text,
            const std::string_view font = "MS UI Gothic",
            const float size            = 24.0f,
            ColorF color                = { 1.0f, 1.0f, 1.0f, 1.0f }
        )
            : m_Handle(nullptr)
        {
            // C-API を呼び出し、初期プロパティを一括で渡す
            m_Handle = n503_renderer2d_text_create(
                text.data(),
                font.data(),
                size,
                color.Red, color.Green, color.Blue, color.Alpha
            );
        }

        /// @brief リソースを解放します。
        ~Text()
        {
            if (m_Handle)
            {
                // 先ほど定義した destroy API を呼び出す
                n503_renderer2d_text_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        // コピー禁止
        Text(const Text&) = delete;
        auto operator=(const Text&) -> Text& = delete;

        // ムーブ許可
        Text(Text&& other) noexcept
            : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(Text&& other) noexcept -> Text&
        {
            if (this != &other)
            {
                if (m_Handle) n503_renderer2d_text_destroy(m_Handle);
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        /// @brief 配置、スケール、回転を設定します。
        auto SetTransform(const Geometry::Transform& transform) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_transform(
                    m_Handle,
                    transform.Position.X, transform.Position.Y,
                    transform.Scale.X, transform.Scale.Y,
                    transform.Rotation
                );
            }
        }

        /// @brief テキストの色を設定します。
        auto SetColor(const ColorF color) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_color(m_Handle, color.Red, color.Green, color.Blue, color.Alpha);
            }
        }

        /// @brief 表示・非表示を切り替えます。
        auto SetVisible(bool visible) -> void
        {
            if (m_Handle)
            {
                n503_renderer2d_text_set_visible(m_Handle, visible ? 1 : 0);
            }
        }

        // 必要に応じて将来的に追加可能なメソッド例：
        // auto SetString(const std::string_view text) -> void;

    private:
        /// @brief DLL 内部で管理されるテキストエンティティへのハンドル
        n503_renderer2d_text_h m_Handle;
    };

} // namespace N503::Renderer2D
