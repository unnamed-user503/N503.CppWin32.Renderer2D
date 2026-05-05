#include "Pch.hpp"
#include "RendererSystem.hpp"

// 1. Project Headers
#include "../Canvas/Font/Atlas.hpp"
#include "../Canvas/Session.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies
#include <algorithm>

namespace N503::Renderer2D::System
{
    auto RendererSystem::Update(Registry& registry, Canvas::Session& session) -> void
    {
        std::array<std::vector<DrawCommand>, static_cast<size_t>(RenderGroup::Threshold)> renderGroups;

        using namespace Component;

        // ------------------------------------------------------------------
        // 1. Sprite Draw
        // ------------------------------------------------------------------
        for (auto entity : registry.GetView<Transform, Sprite, Color>())
        {
            auto& color = registry.GetComponent<Color>(entity);
            if (color.Alpha <= 0.0f)
            {
                continue;
            }

            auto& sprite = registry.GetComponent<Sprite>(entity);
            if (!sprite.Bitmap)
            {
                continue;
            }

            auto& transform = registry.GetComponent<Transform>(entity);

            if (transform.IsDirty)
            {
                // 1. 画像のサイズを取得 (SourceRectが切り出し範囲ならそれが一番正確)
                float w = sprite.SourceRect.right - sprite.SourceRect.left;
                float h = sprite.SourceRect.bottom - sprite.SourceRect.top;

                // 2. 行列計算: 中心オフセット -> スケール -> 回転 -> 移動
                // 32768個回すなら、ここの計算が Dirty フラグでスキップされるのは非常にデカい！
                m_TransformCache[entity] = D2D1::Matrix3x2F::Translation(-w * 0.5f, -h * 0.5f) * D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) *
                                           D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                                           D2D1::Matrix3x2F::Translation(transform.Position.X, transform.Position.Y);

                transform.IsDirty = false;
            }

            // 描画コマンドの作成
            renderGroups[static_cast<size_t>(sprite.Group)].emplace_back(DrawCommand{
                .Bitmap = sprite.Bitmap.get(),
                // Matrix側で座標が決まるので、ここは (0, 0, w, h) でOK
                .DestinationRect = { 0.0f,
                                     0.0f,
                                     static_cast<float>(sprite.SourceRect.right - sprite.SourceRect.left),
                                     static_cast<float>(sprite.SourceRect.bottom - sprite.SourceRect.top) },
                .SourceRect      = sprite.SourceRect,
                .Color           = D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha),
                .Matrix          = m_TransformCache[entity], // これを D2D の pRenderTarget->SetTransform() に渡す
            });
        }

        // ------------------------------------------------------------------
        // 2. Text Draw
        // ------------------------------------------------------------------
        // --- RendererSystem::Update 内の Text Draw ループ (RendererSystem.cpp) ---
        for (auto entity : registry.GetView<Transform, Text, Color>())
        {
            const auto& text = registry.GetComponent<Text>(entity);
            if (!text.Atlas)
            {
                continue;
            }

            const auto& transform = registry.GetComponent<Transform>(entity);
            auto& targetGroup     = renderGroups[static_cast<size_t>(text.Group)];
            const auto& color     = registry.GetComponent<Color>(entity);

            float penX = 0.0f;

            for (const wchar_t wc : text.Content)
            {
                const auto* glyph = text.Atlas->GetGlyph(static_cast<char32_t>(wc));
                if (!glyph)
                {
                    penX += text.FontSize * 0.5f + text.LetterSpacing;
                    continue;
                }

                if (glyph->AdvanceWidth <= 0.0f)
                {
                    continue;
                }

                const float cellW = static_cast<float>(glyph->SourceRect.right - glyph->SourceRect.left);
                const float cellH = static_cast<float>(glyph->SourceRect.bottom - glyph->SourceRect.top);

                // 回転・スケーリングの軸 (文字インクの中央、かつベースライン上)[cite: 9]
                const float pivotX = cellW * 0.5f;
                const float pivotY = glyph->BearingY;

                const D2D1_MATRIX_3X2_F
                    matrix = D2D1::Matrix3x2F::Translation(-pivotX, -pivotY) * D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) *
                             D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                             D2D1::Matrix3x2F::Translation(transform.Position.X + (penX + glyph->BearingX + pivotX) * transform.Scale.X, transform.Position.Y);

                targetGroup.emplace_back(DrawCommand{
                    .Bitmap          = text.Atlas->GetBitmap(),
                    .DestinationRect = D2D1::RectF(0.0f, 0.0f, cellW, cellH),
                    .SourceRect      = glyph->SourceRect,
                    .Color           = D2D1_COLOR_F{ color.Red, color.Green, color.Blue, color.Alpha },
                    .Matrix          = matrix,
                });

                // ペン送りは「インクの幅」ではなく、フォント定義の「AdvanceWidth」を使う[cite: 9]
                penX += glyph->AdvanceWidth + text.LetterSpacing;
            }
        }

        // ------------------------------------------------------------------
        // 3. Sprite Batch
        // ------------------------------------------------------------------
        auto spriteBatch = session.GetDefaultSpriteBatch();

        const auto beforeAntialiasMode = session.GetAntialiasMode();
        session.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        for (auto& commands : renderGroups)
        {
            if (commands.empty())
            {
                continue;
            }

            std::sort(commands.begin(), commands.end(), [](const DrawCommand& a, const DrawCommand& b) { return a.Bitmap < b.Bitmap; });

            for (std::size_t i = 0; i < commands.size();)
            {
                ID2D1Bitmap* currentBitmap   = commands[i].Bitmap;
                const std::size_t batchStart = i;

                while (i < commands.size() && commands[i].Bitmap == currentBitmap)
                {
                    ++i;
                }

                const std::size_t batchSize = i - batchStart;

                spriteBatch->Clear();

                session.AddSprites(
                    spriteBatch,
                    static_cast<uint32_t>(batchSize),
                    &commands[batchStart].DestinationRect,
                    &commands[batchStart].SourceRect,
                    &commands[batchStart].Color,
                    &commands[batchStart].Matrix,
                    sizeof(DrawCommand),
                    sizeof(DrawCommand),
                    sizeof(DrawCommand),
                    sizeof(DrawCommand)
                );

                session.DrawSpriteBatch(spriteBatch, currentBitmap, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);
            }
        }

        session.SetAntialiasMode(beforeAntialiasMode);
        session.ResetTransform();
    }

} // namespace N503::Renderer2D::System
