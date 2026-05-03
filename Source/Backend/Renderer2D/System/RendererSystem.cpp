#include "Pch.hpp"
#include "RendererSystem.hpp"

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies
#include <algorithm> // std::sort のため

namespace N503::Renderer2D::System
{
    auto RendererSystem::Update(Registry& registry, Device::Context& context) -> void
    {
        using namespace Component;

        // RenderGroup ごとに DrawCommand を格納するバッファ
        std::array<std::vector<DrawCommand>, static_cast<size_t>(RenderGroup::Threshold)> renderGroups;

        // 1. 全エンティティを走査してコマンドを収集
        for (auto entity : registry.GetView<Component::Transform, Sprite>())
        {
            auto& color = registry.GetComponent<Color>(entity);
            if (color.Alpha <= 0.0f) continue;

            auto& sprite = registry.GetComponent<Sprite>(entity);
            if (!sprite.Bitmap) continue;

            auto& transform = registry.GetComponent<Transform>(entity);

            // トランスフォームキャッシュの更新
            if (transform.IsDirty)
            {
                // スプライトの中心を基準とした行列合成
                m_TransformCache[entity] = D2D1::Matrix3x2F::Translation(-sprite.DestinationRect.right * 0.5f, -sprite.DestinationRect.bottom * 0.5f) *
                                           D2D1::Matrix3x2F::Scale(transform.Scale.X, transform.Scale.Y) * 
                                           D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                                           D2D1::Matrix3x2F::Translation(transform.Position.X, transform.Position.Y);
                transform.IsDirty = false;
            }

            renderGroups[static_cast<size_t>(sprite.Group)].emplace_back(DrawCommand{
                .Bitmap          = sprite.Bitmap.get(),
                .DestinationRect = sprite.DestinationRect,
                .SourceRect      = sprite.SourceRect,
                .Matrix          = m_TransformCache[entity],
                .Color           = D2D1::ColorF(color.Red, color.Green, color.Blue, color.Alpha),
            });
        }

        auto d2dContext3 = context.GetD2DContext3();
        auto spriteBatch = context.GetSpriteBatch();

        const auto beforeAntialiasMode = d2dContext3->GetAntialiasMode();
        d2dContext3->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        // 2. RenderGroup の順番に処理[cite: 10]
        for (auto& commands : renderGroups)
        {
            if (commands.empty()) continue;

            // --- 重要: Bitmap ポインタでソートして、同じ Bitmap を隣接させる ---
            std::sort(commands.begin(), commands.end(), [](const DrawCommand& a, const DrawCommand& b) {
                return a.Bitmap < b.Bitmap;
            });

            // 3. 同じ Bitmap を持つ連続した区間をバッチとして描画
            for (size_t i = 0; i < commands.size(); )
            {
                ID2D1Bitmap* currentBitmap = commands[i].Bitmap;
                size_t batchStart = i;

                // 同じビットマップが続く限りインデックスを進める
                while (i < commands.size() && commands[i].Bitmap == currentBitmap)
                {
                    i++;
                }

                size_t batchSize = i - batchStart;

                // スプライトバッチをクリアして、同じビットマップのものを一括登録
                spriteBatch->Clear();

                // AddSprites のストライド機能を利用して DrawCommand 構造体の配列をそのまま流し込む
                spriteBatch->AddSprites(
                    static_cast<uint32_t>(batchSize),
                    &commands[batchStart].DestinationRect,
                    &commands[batchStart].SourceRect,
                    &commands[batchStart].Color,
                    &commands[batchStart].Matrix,
                    sizeof(DrawCommand), // 各データのストライド（構造体のサイズ）を指定
                    sizeof(DrawCommand),
                    sizeof(DrawCommand),
                    sizeof(DrawCommand)
                );

                // バッチ描画の実行[cite: 10]
                d2dContext3->DrawSpriteBatch(
                    spriteBatch.get(),
                    currentBitmap,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                    D2D1_SPRITE_OPTIONS_NONE
                );
            }
        }

        d2dContext3->SetAntialiasMode(beforeAntialiasMode);
        context.SetTransform(D2D1::Matrix3x2F::Identity());
    }
}
