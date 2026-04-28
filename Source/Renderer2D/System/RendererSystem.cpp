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
                    float centerX     = sprite.Width * 0.5f;
                    float centerY     = sprite.Height * 0.5f;
                    sprite.Transform  = D2D1::Matrix3x2F::Translation(-centerX, -centerY) * D2D1::Matrix3x2F::Scale(transform.ScaleX, transform.ScaleY) *
                                        D2D1::Matrix3x2F::Rotation(transform.Rotation) * D2D1::Matrix3x2F::Translation(transform.X, transform.Y);
                    transform.IsDirty = false;
                }

                context.SetTransform(sprite.Transform);
                context.GetD2DContext()->DrawBitmap(sprite.Bitmap.get(), D2D1::RectF(0, 0, sprite.Width, sprite.Height));
            }
        }

        // 行列をリセット
        context.SetTransform(D2D1::Matrix3x2F::Identity());
    }

} // namespace N503::Renderer2D::System
