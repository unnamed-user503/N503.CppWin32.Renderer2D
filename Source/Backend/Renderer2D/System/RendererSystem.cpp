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
        for (auto entity : registry.GetView<Transform, Sprite, Visible, Color>())
        {
            auto& visible = registry.GetComponent<Visible>(entity);

            if (!visible.IsEnabled)
            {
                continue;
            }

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
                float w = sprite.SourceRect.right - sprite.SourceRect.left;
                float h = sprite.SourceRect.bottom - sprite.SourceRect.top;

                m_TransformCache[entity] = D2D1::Matrix3x2F::Translation(-w * 0.5f, -h * 0.5f) * D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) * D2D1::Matrix3x2F::Rotation(transform.Rotation) * D2D1::Matrix3x2F::Translation(transform.Position.X, transform.Position.Y);

                transform.IsDirty = false;
            }

            renderGroups[static_cast<size_t>(sprite.Group)].emplace_back(DrawCommand{
                .Bitmap          = sprite.Bitmap.get(),
                .DestinationRect = { 0.0f, 0.0f, static_cast<float>(sprite.SourceRect.right - sprite.SourceRect.left), static_cast<float>(sprite.SourceRect.bottom - sprite.SourceRect.top) },
                .SourceRect      = sprite.SourceRect,
                .Color           = D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha),
                .Matrix          = m_TransformCache[entity],
            });
        }

        // ------------------------------------------------------------------
        // 2. Text Draw
        // ------------------------------------------------------------------
        for (auto entity : registry.GetView<Transform, Text, Visible, Color>())
        {
            auto& visible = registry.GetComponent<Visible>(entity);

            if (!visible.IsEnabled)
            {
                continue;
            }

            const auto& text = registry.GetComponent<Text>(entity);

            if (!text.Atlas)
            {
                continue;
            }

            if (text.VisibleCount == 0)
            {
                continue;
            }

            const auto& transform = registry.GetComponent<Transform>(entity);
            auto& targetGroup     = renderGroups[static_cast<size_t>(text.Group)];
            const auto& color     = registry.GetComponent<Color>(entity);

            const float lineHeight = text.Atlas->GetLineHeight() * text.LineHeightScale;
            const bool doWrap      = text.WrapWidth > 0.0f;

            float penX = 0.0f;
            float penY = 0.0f;

            std::int32_t drawnCount = 0;

            // ---- 単語単位の折り返しのために単語境界を事前スキャン ----
            // wchar_t の範囲で「次の折り返し可能位置までの幅」を計算して折り返しを判断する
            const auto measureToNextBreak = [&](std::size_t startIndex) -> float
            {
                float w = 0.0f;
                for (std::size_t i = startIndex; i < text.Content.size(); ++i)
                {
                    const wchar_t wc = text.Content[i];
                    // スペース・改行・CJK文字は折り返し可能位置
                    if (wc == L' ' || wc == L'\n' || wc == L'\t' || (wc >= 0x3000 && wc <= 0x9FFF) || // CJK
                        (wc >= 0xFF00 && wc <= 0xFFEF))                                               // 全角
                    {
                        break;
                    }
                    const auto* g  = text.Atlas->GetGlyph(static_cast<char32_t>(wc));
                    w             += g ? g->AdvanceWidth + text.LetterSpacing : text.FontSize * 0.5f + text.LetterSpacing;
                }
                return w;
            };

            for (std::size_t idx = 0; idx < text.Content.size(); ++idx)
            {
                if (text.VisibleCount >= 0 && drawnCount >= text.VisibleCount)
                {
                    break;
                }

                const wchar_t wc = text.Content[idx];

                // 明示的改行
                if (wc == L'\n')
                {
                    penX  = 0.0f;
                    penY += lineHeight;
                    continue;
                }

                // タブ (スペース4文字分)
                if (wc == L'\t')
                {
                    const auto* spGlyph  = text.Atlas->GetGlyph(U' ');
                    const float spW      = spGlyph ? spGlyph->AdvanceWidth : text.FontSize * 0.5f;
                    penX                += spW * 4.0f + text.LetterSpacing;

                    if (doWrap && penX > text.WrapWidth)
                    {
                        penX  = 0.0f;
                        penY += lineHeight;
                    }
                    continue;
                }

                const auto* glyph = text.Atlas->GetGlyph(static_cast<char32_t>(wc));

                // アトラスにない文字はスペース扱い
                if (!glyph)
                {
                    penX += text.FontSize * 0.5f + text.LetterSpacing;
                    if (doWrap && penX > text.WrapWidth)
                    {
                        penX  = 0.0f;
                        penY += lineHeight;
                    }
                    continue;
                }

                if (glyph->AdvanceWidth <= 0.0f)
                {
                    continue;
                }

                // ---- 折り返し判定 ----
                if (doWrap)
                {
                    // スペースは折り返し可能位置: 次の単語が収まらなければ改行
                    if (wc == L' ')
                    {
                        const float nextWordW = measureToNextBreak(idx + 1);
                        if (penX + glyph->AdvanceWidth + nextWordW > text.WrapWidth)
                        {
                            penX  = 0.0f;
                            penY += lineHeight;
                            continue; // スペース自体は行頭に出力しない
                        }
                    }
                    // CJK・全角は文字単体で折り返し可能
                    else if ((wc >= 0x3000 && wc <= 0x9FFF) || (wc >= 0xFF00 && wc <= 0xFFEF))
                    {
                        if (penX + glyph->AdvanceWidth > text.WrapWidth)
                        {
                            penX  = 0.0f;
                            penY += lineHeight;
                        }
                    }
                    // ASCII 単語: 行頭でもなく次の文字まで収まらない場合
                    else if (penX > 0.0f && penX + glyph->AdvanceWidth > text.WrapWidth)
                    {
                        penX  = 0.0f;
                        penY += lineHeight;
                    }
                }

                const float cellW  = static_cast<float>(glyph->SourceRect.right - glyph->SourceRect.left);
                const float cellH  = static_cast<float>(glyph->SourceRect.bottom - glyph->SourceRect.top);
                const float pivotX = cellW * 0.5f;
                const float pivotY = glyph->BearingY;

                const D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(-pivotX, -pivotY) * D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) * D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                                                 D2D1::Matrix3x2F::Translation(transform.Position.X + (penX + glyph->BearingX + pivotX) * transform.Scale.X, transform.Position.Y + penY * transform.Scale.Y);

                targetGroup.emplace_back(DrawCommand{
                    .Bitmap          = text.Atlas->GetBitmap(),
                    .DestinationRect = D2D1::RectF(0.0f, 0.0f, cellW, cellH),
                    .SourceRect      = glyph->SourceRect,
                    .Color           = D2D1_COLOR_F{ color.Red, color.Green, color.Blue, color.Alpha },
                    .Matrix          = matrix,
                });

                penX += glyph->AdvanceWidth + text.LetterSpacing;

                ++drawnCount;
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

                session.AddSprites(spriteBatch, static_cast<uint32_t>(batchSize), &commands[batchStart].DestinationRect, &commands[batchStart].SourceRect, &commands[batchStart].Color, &commands[batchStart].Matrix, sizeof(DrawCommand), sizeof(DrawCommand), sizeof(DrawCommand), sizeof(DrawCommand));

                session.DrawSpriteBatch(spriteBatch, currentBitmap, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);
            }
        }

        session.SetAntialiasMode(beforeAntialiasMode);
        session.ResetTransform();
    }

} // namespace N503::Renderer2D::System
