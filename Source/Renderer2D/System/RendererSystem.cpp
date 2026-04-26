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

namespace N503::Renderer2D::System
{

    auto RendererSystem::Update(Registry& registry, Device::Context& context) -> void
    {
        for (auto e : registry.GetView<Transform, Sprite>())
        {
            auto& transform = registry.GetComponent<Transform>(e);
            auto& sprite    = registry.GetComponent<Sprite>(e);

            if (sprite.Bitmap)
            {
                // 1. 行列を作成（MSのD2D1名前空間のヘルパーを使うと楽です）
                // 拡大 -> 回転 -> 移動 の順序を死守！
                // 中心点を計算（例：画像の真ん中）
                float centerX = sprite.Width / 2.0f;
                float centerY = sprite.Height / 2.0f;

                // 1. 原点を中心に持ってくる (-centerX, -centerY)
                // 2. 拡大・回転
                // 3. 画面上の目的地へ移動 (transform.X, transform.Y)
                auto matrix = D2D1::Matrix3x2F::Translation(-centerX, -centerY) * D2D1::Matrix3x2F::Scale(transform.ScaleX, transform.ScaleY) *
                              D2D1::Matrix3x2F::Rotation(transform.Rotation) * D2D1::Matrix3x2F::Translation(transform.X, transform.Y);

                // 2. 行列を適用
                context.SetTransform(matrix);

                // 3. 描画矩形（ローカル座標）
                // ここに transform.X を入れてはいけません。
                // (0, 0) を基準とした「絵そのもののサイズ」を指定します。
                D2D1_RECT_F destRect = { 0, 0, sprite.Width, sprite.Height };

                context.DrawBitmap(sprite.Bitmap, destRect);
            }
        }

        // 次の描画に影響しないよう、行列をリセット（単位行列）
        context.SetTransform(D2D1::Matrix3x2F::Identity());
    }

} // namespace N503::Renderer2D::System
