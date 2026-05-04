#include "Pch.hpp"
#include "RendererSystem.hpp"

// 1. Project Headers
#include "../Canvas/FontAtlas.hpp"
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

        CollectSpriteCommands(registry, renderGroups);
        CollectTextCommands(registry, renderGroups);
        FlushRenderGroups(session, renderGroups);
    }

    // =========================================================
    // Sprite コマンド収集
    // =========================================================

    auto RendererSystem::CollectSpriteCommands(
        Registry& registry, std::array<std::vector<DrawCommand>, static_cast<size_t>(RenderGroup::Threshold)>& renderGroups
    ) -> void
    {
        using namespace Component;

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
                m_TransformCache[entity] = D2D1::Matrix3x2F::Translation(-sprite.DestinationRect.right * 0.5f, -sprite.DestinationRect.bottom * 0.5f) *
                                           D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) * D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                                           D2D1::Matrix3x2F::Translation(transform.Position.X, transform.Position.Y);
                transform.IsDirty = false;
            }

            renderGroups[static_cast<size_t>(sprite.Group)].emplace_back(DrawCommand{
                .Bitmap          = sprite.Bitmap.get(),
                .DestinationRect = sprite.DestinationRect,
                .SourceRect      = sprite.SourceRect,
                .Color           = D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha),
                .Matrix          = m_TransformCache[entity],
            });
        }
    }

    // =========================================================
    // Text コマンド収集
    // グリフ子エンティティを生成せず、直接 DrawCommand に変換する
    // =========================================================

    auto RendererSystem::CollectTextCommands(
        Registry& registry, std::array<std::vector<DrawCommand>, static_cast<size_t>(RenderGroup::Threshold)>& renderGroups
    ) -> void
    {
        using namespace Component;

        for (auto entity : registry.GetView<Transform, Text, Color>())
        {
            const auto& text = registry.GetComponent<Text>(entity);
            if (!text.Atlas)
            {
                continue;
            }

            const auto& transform = registry.GetComponent<Transform>(entity);

            // ★ Group に応じて正しいバケツへ振り分け (Sprite と同じ扱い)
            auto& targetGroup = renderGroups[static_cast<size_t>(text.Group)];

            float penX = 0.0f;

            const auto& color = registry.GetComponent<Color>(entity);

            for (const wchar_t wc : text.Content)
            {
                const Canvas::GlyphInfo* glyph = text.Atlas->GetGlyph(static_cast<char32_t>(wc));
                if (!glyph)
                {
                    continue;
                }

                if (glyph->AdvanceWidth <= 0.0f)
                {
                    penX += text.LetterSpacing;
                    continue;
                }

                const float glyphW = glyph->SourceRect.right - glyph->SourceRect.left;
                const float glyphH = glyph->SourceRect.bottom - glyph->SourceRect.top;

                const D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(-glyphW * 0.5f, -glyphH * 0.5f) *
                                                 D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) *
                                                 D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                                                 D2D1::Matrix3x2F::Translation(
                                                     transform.Position.X + penX, // ← BearingX を除去
                                                     transform.Position.Y
                                                 );

                // ★ text.Atlas->GetBitmap() がフォント×サイズごとに異なる Bitmap を返す
                //    FlushRenderGroups の Bitmap ソートで自動的にバッチ化される
                targetGroup.emplace_back(DrawCommand{
                    .Bitmap          = text.Atlas->GetBitmap(),
                    .DestinationRect = D2D1::RectF(0.0f, 0.0f, glyphW, glyphH),
                    .SourceRect      = D2D1::RectU(
                        static_cast<UINT32>(glyph->SourceRect.left),
                        static_cast<UINT32>(glyph->SourceRect.top),
                        static_cast<UINT32>(glyph->SourceRect.right),
                        static_cast<UINT32>(glyph->SourceRect.bottom)
                    ),
                    .Color  = D2D1_COLOR_F{ color.Red, color.Green, color.Blue, color.Alpha },
                    .Matrix = matrix,
                });

                penX += glyph->AdvanceWidth + text.LetterSpacing;
            }
        }
    }

    // =========================================================
    // RenderGroup をフラッシュして描画
    // =========================================================

    auto RendererSystem::FlushRenderGroups(
        Canvas::Session& session, std::array<std::vector<DrawCommand>, static_cast<size_t>(RenderGroup::Threshold)>& renderGroups
    ) -> void
    {
        auto spriteBatch = session.GetDefaultSpriteBatch();

        const auto beforeAntialiasMode = session.GetAntialiasMode();
        session.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        for (auto& commands : renderGroups)
        {
            if (commands.empty())
            {
                continue;
            }

            // 同じ Bitmap を隣接させてバッチ効率を上げる
            std::sort(commands.begin(), commands.end(), [](const DrawCommand& a, const DrawCommand& b) { return a.Bitmap < b.Bitmap; });

            for (size_t i = 0; i < commands.size();)
            {
                ID2D1Bitmap* currentBitmap = commands[i].Bitmap;
                const size_t batchStart    = i;

                while (i < commands.size() && commands[i].Bitmap == currentBitmap)
                {
                    ++i;
                }

                const size_t batchSize = i - batchStart;

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
