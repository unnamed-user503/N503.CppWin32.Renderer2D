#include "Pch.hpp"
#include "RendererSystem.hpp"

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <map>

namespace N503::Renderer2D::System
{

    auto RendererSystem::Update(Registry& registry, Device::Context& context) -> void
    {
        // 1. 各レイヤー（RenderGroup）ごとのバケツを用意
        //    ※ RendererSystemのメンバ変数にして clear() して使い回すとさらに高速です
        std::array<std::vector<Entity>, (size_t)RenderGroup::Limited> buckets;

        for (auto entity : registry.GetView<Transform, Sprite>())
        {
            auto& sprite = registry.GetComponent<Sprite>(entity);
            buckets[static_cast<size_t>(sprite.Group)].push_back(entity);
        }

        auto d2dContext3 = context.GetD2DContext3();
        auto spriteBatch = context.GetSpriteBatch();

        // 2. レイヤーごとに順番に描画（Background -> World -> UI...）
        for (size_t i = 0; i < buckets.size(); ++i)
        {
            auto& layerList = buckets[i];
            if (layerList.empty())
            {
                continue;
            }

            // --- A. レイヤー内での Zソート ---
            std::sort(
                layerList.begin(),
                layerList.end(),
                [&](Entity a, Entity b) { return registry.GetComponent<Transform>(a).Z < registry.GetComponent<Transform>(b).Z; }
            );

            // --- B. 描画戦略の分岐 ---
            if (i != static_cast<size_t>(RenderGroup::Background))
            {
                // 【背景レイヤー：SpriteBatch で一括描画】

                // 既にレイヤー内のエンティティは layerList にあるので、
                // ビットマップごとにグループ化してバッチ処理を行う
                std::map<ID2D1Bitmap*, std::vector<Entity>> groups;
                for (auto entity : layerList)
                {
                    auto& sprite = registry.GetComponent<Sprite>(entity);
                    if (sprite.Bitmap)
                    {
                        groups[sprite.Bitmap.get()].push_back(entity);
                    }
                }

                auto oldAntialiasMode = d2dContext3->GetAntialiasMode();
                d2dContext3->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
                for (auto& [bitmap, entities] : groups)
                {
                    spriteBatch->Clear();

                    // バッファの用意
                    std::vector<D2D1_RECT_F> destinations;
                    std::vector<D2D1_MATRIX_3X2_F> matrixes;
                    destinations.reserve(entities.size());
                    matrixes.reserve(entities.size());

                    for (auto entity : entities)
                    {
                        auto& transform = registry.GetComponent<Transform>(entity);
                        auto& sprite    = registry.GetComponent<Sprite>(entity);

                        // 行列の更新（Dirtyフラグ制御）
                        if (transform.IsDirty)
                        {
                            float cX         = sprite.Width * 0.5f;
                            float cY         = sprite.Height * 0.5f;
                            sprite.Transform = D2D1::Matrix3x2F::Translation(-cX, -cY) * D2D1::Matrix3x2F::Scale(transform.ScaleX, transform.ScaleY) *
                                               D2D1::Matrix3x2F::Rotation(transform.Rotation) * D2D1::Matrix3x2F::Translation(transform.X, transform.Y);
                            transform.IsDirty = false;
                        }

                        destinations.push_back({ 0, 0, sprite.Width, sprite.Height });
                        matrixes.push_back(sprite.Transform);
                    }

                    spriteBatch->AddSprites(
                        static_cast<UINT32>(entities.size()),
                        destinations.data(), // 第2引数
                        nullptr,             // 第3引数 (SourceRects)
                        nullptr,             // 第4引数 (Colors)
                        matrixes.data()      // 第5引数 (Transforms)
                    );

                    d2dContext3->DrawSpriteBatch(spriteBatch.get(), bitmap, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);
                }
                d2dContext3->SetAntialiasMode(oldAntialiasMode);
            }
            else
            {
                // 【その他のレイヤー：通常の DrawBitmap 描画】
                // Z順を守るため、ビットマップが違ってもそのままループで描画する
                for (auto entity : layerList)
                {
                    auto& transform = registry.GetComponent<Transform>(entity);
                    auto& sprite    = registry.GetComponent<Sprite>(entity);

                    if (!sprite.Bitmap)
                    {
                        continue;
                    }

                    if (transform.IsDirty)
                    {
                        float cX         = sprite.Width * 0.5f;
                        float cY         = sprite.Height * 0.5f;
                        sprite.Transform = D2D1::Matrix3x2F::Translation(-cX, -cY) * D2D1::Matrix3x2F::Scale(transform.ScaleX, transform.ScaleY) *
                                           D2D1::Matrix3x2F::Rotation(transform.Rotation) * D2D1::Matrix3x2F::Translation(transform.X, transform.Y);
                        transform.IsDirty = false;
                    }

                    context.SetTransform(sprite.Transform);
                    context.GetD2DContext()->DrawBitmap(sprite.Bitmap.get(), D2D1::RectF(0, 0, sprite.Width, sprite.Height));
                }
            }
        }

        // 行列をリセット
        context.SetTransform(D2D1::Matrix3x2F::Identity());
    }

} // namespace N503::Renderer2D::System
