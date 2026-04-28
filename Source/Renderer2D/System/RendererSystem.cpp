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
#include <format>
#include <iostream>

namespace N503::Renderer2D::System
{

    RendererSystem::RendererSystem()
    {
    }

    auto RendererSystem::Update(Registry& registry, Device::Context& context) -> void
    {
        std::array<std::vector<DrawCommand>, static_cast<size_t>(RenderGroup::Threshold)> renderGroup;

        for (auto entity : registry.GetView<Transform, Sprite>())
        {
            auto& transform = registry.GetComponent<Transform>(entity);
            auto& sprite    = registry.GetComponent<Sprite>(entity);

            if (!sprite.Bitmap)
            {
                continue;
            }

            if (transform.IsDirty)
            {
                m_TransformCache[entity] = D2D1::Matrix3x2F::Translation(-sprite.DestinationRect.right * 0.5f, -sprite.DestinationRect.bottom * 0.5f) *
                                           D2D1::Matrix3x2F::Scale(transform.ScaleX, transform.ScaleY) * D2D1::Matrix3x2F::Rotation(transform.Rotation) *
                                           D2D1::Matrix3x2F::Translation(transform.X, transform.Y);
                transform.IsDirty = false;
            }

            renderGroup[static_cast<size_t>(sprite.Group)].emplace_back(DrawCommand{
                .Bitmap = sprite.Bitmap.get(), .DestinationRect = sprite.DestinationRect, .Matrix = m_TransformCache[entity] });
        }

        auto d2dContext3 = context.GetD2DContext3();
        auto spriteBatch = context.GetSpriteBatch();

        const auto oldAntialiasMode = d2dContext3->GetAntialiasMode();
        d2dContext3->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        for (int i = 0; i < static_cast<int>(RenderGroup::Threshold); ++i)
        {
            if (renderGroup[i].empty())
            {
                continue;
            }

            spriteBatch->Clear();

            for (const auto& command : renderGroup[i])
            {
                spriteBatch->AddSprites(
                    1,                        // スプライト数
                    &command.DestinationRect, // 描画先
                    nullptr,                  // ソース矩形（nullptrならビットマップ全体）
                    nullptr,                  // 色（nullptrなら白/不透明）
                    &command.Matrix,          // 個別のトランスフォーム
                    sizeof(D2D1_RECT_F),      // ストライド
                    0,                        // ストライド
                    0,                        // ストライド
                    sizeof(D2D1_MATRIX_3X2_F) // ストライド
                );
            }

            if (!renderGroup[i].empty())
            {
                d2dContext3->DrawSpriteBatch(
                    spriteBatch.get(),
                    renderGroup[i][0].Bitmap, // 共通のビットマップ
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                    D2D1_SPRITE_OPTIONS_NONE
                );
            }
        }

        d2dContext3->SetAntialiasMode(oldAntialiasMode);

        context.SetTransform(D2D1::Matrix3x2F::Identity());
    }

} // namespace N503::Renderer2D::System
