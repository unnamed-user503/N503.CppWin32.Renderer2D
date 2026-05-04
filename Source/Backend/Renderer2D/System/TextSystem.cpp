#include "Pch.hpp"
#include "TextSystem.hpp"

// 1. Project Headers
#include "Component/Color.hpp"
#include "Component/Sprite.hpp"
#include "Component/Text.hpp"
#include "Component/Transform.hpp"
#include "Registry.hpp"

namespace N503::Renderer2D::System
{
    auto TextSystem::Update(Registry& registry) -> void
    {
        for (auto textEntity : registry.GetView<Component::Text, Component::Transform, Component::Color>())
        {
            auto& text = registry.GetComponent<Component::Text>(textEntity);

            if (!text.IsDirty)
            {
                continue;
            }

            // 前回のグリフ子エンティティを破棄
            DestroyGlyphs(registry, textEntity);

            // アトラスが未設定なら何もしない
            if (!text.Atlas)
            {
                text.IsDirty = false;
                continue;
            }

            // グリフ子エンティティを生成
            BuildGlyphs(registry, textEntity);

            text.IsDirty = false;
        }
    }

    auto TextSystem::DestroyGlyphs(Registry& registry, Entity textEntity) -> void
    {
        auto it = m_GlyphEntities.find(textEntity);
        if (it == m_GlyphEntities.end())
        {
            return;
        }

        for (auto glyph : it->second)
        {
            registry.DestroyEntity(glyph);
        }

        it->second.clear();
    }

    auto TextSystem::BuildGlyphs(Registry& registry, Entity textEntity) -> void
    {
        using namespace Component;

        const auto& text          = registry.GetComponent<Text>(textEntity);
        const auto& baseTransform = registry.GetComponent<Transform>(textEntity);
        const auto& baseColor     = registry.GetComponent<Color>(textEntity);

        auto& glyphs = m_GlyphEntities[textEntity];

        float penX = 0.0f;

        for (const char32_t cp : text.Content)
        {
            const Canvas::Glyph* glyph = text.Atlas->GetGlyph(static_cast<char32_t>(cp));
            if (!glyph)
            {
                // アトラスに存在しない文字 (スペース等) は固定幅で送る
                penX += text.FontSize * 0.5f + text.LetterSpacing;
                continue;
            }

            // 描画不要なグリフ (スペース等) はペンだけ進める
            // 修正: スペースは AdvanceWidth だけ正しく進める
            if (glyph->AdvanceWidth <= 0.0f)
            {
                penX += text.LetterSpacing;
                continue;
            }
            // AdvanceWidth > 0 のグリフは末尾で正しく進める
            penX += glyph->AdvanceWidth + text.LetterSpacing;

            const float glyphW = glyph->SourceRect.right - glyph->SourceRect.left;
            const float glyphH = glyph->SourceRect.bottom - glyph->SourceRect.top;

            auto glyphEntity = registry.CreateEntity();
            glyphs.push_back(glyphEntity);

            // Sprite: アトラスの該当領域を参照
            registry.AddComponent<Sprite>(
                glyphEntity,
                Sprite{
                    .Bitmap          = text.Atlas->GetBitmap(),
                    .Group           = RenderGroup::Text,
                    .DestinationRect = D2D1::RectF(0.0f, 0.0f, glyphW, glyphH),
                    .SourceRect      = D2D1::RectU(
                        static_cast<UINT32>(glyph->SourceRect.left),
                        static_cast<UINT32>(glyph->SourceRect.top),
                        static_cast<UINT32>(glyph->SourceRect.right),
                        static_cast<UINT32>(glyph->SourceRect.bottom)
                    ),
                }
            );

            // Transform: 親の位置 + ペン位置オフセット
            registry.AddComponent<Transform>(glyphEntity, Transform{
                .Position = {
                    baseTransform.Position.X + penX + glyph->BearingX,
                    baseTransform.Position.Y,
                    baseTransform.Position.Z,
                },
                .Rotation = baseTransform.Rotation,
                .Scale    = baseTransform.Scale,
                .IsDirty  = true,
            });

            // Color: 親から継承
            registry.AddComponent<Color>(glyphEntity, Color{ baseColor.Red, baseColor.Green, baseColor.Blue, baseColor.Alpha });

            penX += glyph->AdvanceWidth + text.LetterSpacing;
        }
    }

} // namespace N503::Renderer2D::System
