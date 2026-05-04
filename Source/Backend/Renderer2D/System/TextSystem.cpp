#include "Pch.hpp"
#include "TextSystem.hpp"

// 1. Project Headers
#include "Component/Color.hpp"
#include "Component/Sprite.hpp"
#include "Component/Text.hpp"
#include "Component/Transform.hpp"
#include "Registry.hpp"

// 2. Project Dependencies
// 3. WIL (Windows Implementation Library)
// 4. Third-party Libraries
// 5. Windows Headers
// 6. C++ Standard Libraries

namespace N503::Renderer2D::System
{
    auto TextSystem::Update(Registry& registry) -> void
    {
        // Text, Transform, Color コンポーネントを持つエンティティを走査
        for (auto textEntity : registry.GetView<Component::Text, Component::Transform, Component::Color>())
        {
            auto& text = registry.GetComponent<Component::Text>(textEntity);

            // テキストに変更（IsDirty）がない場合はスキップ
            if (!text.IsDirty)
            {
                continue;
            }

            // このテキストエンティティに紐づく既存のグリフ（一文字ずつのエンティティ）を検索
            auto it = m_GlyphEntities.find(textEntity);

            if (it == m_GlyphEntities.end())
            {
                return;
            }

            // 新しく作り直すため、以前生成したグリフエンティティをすべて破棄
            for (auto glyph : it->second)
            {
                registry.DestroyEntity(glyph);
            }

            it->second.clear();

            // アトラス（フォントテクスチャ）が設定されていない場合は処理終了
            if (!text.Atlas)
            {
                text.IsDirty = false;
                continue;
            }

            // テキスト全体の基準となるトランスフォームと色を取得
            const auto& baseTransform = registry.GetComponent<Component::Transform>(textEntity);
            const auto& baseColor     = registry.GetComponent<Component::Color>(textEntity);

            auto& glyphs = m_GlyphEntities[textEntity];

            // 文字を描画するX座標のオフセット（ペン先位置）
            float penX = 0.0f;

            // 文字列を一文字ずつループ処理
            for (const char32_t codePage : text.Content)
            {
                // アトラスから対応する文字のメトリクス（配置情報）を取得
                const auto* glyph = text.Atlas->GetGlyph(static_cast<char32_t>(codePage));

                // アトラスに文字がない場合は空白（スペース等）として処理
                if (!glyph)
                {
                    penX += text.FontSize * 0.5f + text.LetterSpacing;
                    continue;
                }

                // 幅がない文字の処理
                if (glyph->AdvanceWidth <= 0.0f)
                {
                    penX += text.LetterSpacing;
                    continue;
                }

                // 描画位置の更新（文字の進み幅を適用）
                penX += glyph->AdvanceWidth + text.LetterSpacing;

                // アトラス内での切り出しサイズを計算
                const float glyphW = glyph->SourceRect.right - glyph->SourceRect.left;
                const float glyphH = glyph->SourceRect.bottom - glyph->SourceRect.top;

                // 一文字を表現する新しいエンティティを作成
                auto glyphEntity = registry.CreateEntity();

                glyphs.push_back(glyphEntity);

                // 文字のテクスチャ情報を Sprite コンポーネントとして追加
                registry.AddComponent<Component::Sprite>(
                    glyphEntity,
                    Component::Sprite{
                        .Bitmap     = text.Atlas->GetBitmap(),
                        .SourceRect = D2D1::RectU(
                            static_cast<UINT32>(glyph->SourceRect.left),
                            static_cast<UINT32>(glyph->SourceRect.top),
                            static_cast<UINT32>(glyph->SourceRect.right),
                            static_cast<UINT32>(glyph->SourceRect.bottom)
                        ),
                        .Group      = RenderGroup::Text,
                    }
                );

                // 文字の個別位置を Transform コンポーネントとして設定
                registry.AddComponent<Component::Transform>(glyphEntity, Component::Transform{
                    .Position = {
                        baseTransform.Position.X + penX + glyph->BearingX, // 基準位置 + 送り量 + 左ベアリング
                        baseTransform.Position.Y,
                        baseTransform.Position.Z,
                    },
                    .Rotation = baseTransform.Rotation,
                    .Scale    = baseTransform.Scale,
                    .IsDirty  = true,
                });

                // 色情報を継承
                registry.AddComponent<Component::Color>(glyphEntity, Component::Color{ baseColor.Red, baseColor.Green, baseColor.Blue, baseColor.Alpha });

                // 次の文字のためにペン先をさらに進める
                penX += glyph->AdvanceWidth + text.LetterSpacing;
            }

            // 更新完了フラグを立てる
            text.IsDirty = false;
        }
    }

} // namespace N503::Renderer2D::System
