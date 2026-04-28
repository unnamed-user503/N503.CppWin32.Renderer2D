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

namespace N503::Renderer2D::System
{
    auto RendererSystem::Update(Registry& registry, Device::Context& context) -> void
    {
        // 1. 各レイヤーの仕分け（m_Bucketsはメンバ変数だが、ここでは単なる「作業用コンテナ」として使用）
        for (auto& bucket : m_Buckets) bucket.clear();

        for (auto entity : registry.GetView<Transform, Sprite>())
        {
            auto& sprite = registry.GetComponent<Sprite>(entity);
            m_Buckets[static_cast<size_t>(sprite.Group)].push_back(entity);
        }

        // 2. 描画コンテキストの準備
        auto d2dContext3 = context.GetD2DContext3();
        auto oldMode = d2dContext3->GetAntialiasMode();
        d2dContext3->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

        // 3. レイヤーごとにソートと描画を実行
        for (size_t i = 0; i < m_Buckets.size(); ++i)
        {
            auto& layerEntities = m_Buckets[i];
            if (layerEntities.empty()) continue;

            // ソートロジック（ここで行列の更新が必要かどうかも判断可能）
            SortLayer(registry, layerEntities, static_cast<RenderGroup>(i));

            // バッチを構築して描画（バッファ m_DestRects, m_Matrices を引数で渡す）
            // Update関数内のループを少し調整
            if (i == static_cast<size_t>(RenderGroup::Background) || layerEntities.size() > 100) 
            {
                // 大量にある、または背景ならバッチ描画（エイリアスOFF）
                ProcessBatchRender(registry, context, layerEntities);
            }
            else 
            {
                // 数が少ない、または重要なレイヤー（Playerなど）は個別描画（エイリアスON）
                ProcessIndividualRender(registry, context, layerEntities);
            }
        }

        d2dContext3->SetAntialiasMode(oldMode);
        context.SetTransform(D2D1::Matrix3x2F::Identity());
    }

    // --- 以下、状態を極力持たないヘルパー関数群 ---

    void RendererSystem::SortLayer(Registry& registry, std::vector<Entity>& entities, RenderGroup group)
    {
        if (group == RenderGroup::Background)
        {
            std::sort(entities.begin(), entities.end(), [&](Entity a, Entity b) {
                return registry.GetComponent<Sprite>(a).Bitmap.get() < registry.GetComponent<Sprite>(b).Bitmap.get();
            });
        }
        else
        {
            std::sort(entities.begin(), entities.end(), [&](Entity a, Entity b) {
                auto& ta = registry.GetComponent<Transform>(a);
                auto& tb = registry.GetComponent<Transform>(b);
                if (ta.Z != tb.Z) return ta.Z < tb.Z;
                return registry.GetComponent<Sprite>(a).Bitmap.get() < registry.GetComponent<Sprite>(b).Bitmap.get();
            });
        }
    }

    void RendererSystem::ProcessBatchRender(Registry& registry, Device::Context& context, const std::vector<Entity>& entities)
    {
        ID2D1Bitmap* currentBitmap = nullptr;
        m_DestRects.clear();
        m_Matrices.clear();

        for (auto entity : entities)
        {
            auto& transform = registry.GetComponent<Transform>(entity);
            auto& sprite    = registry.GetComponent<Sprite>(entity);
            if (!sprite.Bitmap) continue;

            // 必要なら行列を更新
            if (transform.IsDirty)
            {
                UpdateSpriteTransform(transform, sprite);
            }

            // 画像の切り替わりで描画実行
            if (currentBitmap != sprite.Bitmap.get())
            {
                if (currentBitmap)
                {
                    ExecuteDrawBatch(context, currentBitmap, m_DestRects, m_Matrices);
                }
                currentBitmap = sprite.Bitmap.get();
                m_DestRects.clear();
                m_Matrices.clear();
            }

            m_DestRects.push_back({ 0, 0, sprite.Width, sprite.Height });
            m_Matrices.push_back(sprite.Transform);
        }

        if (currentBitmap)
        {
            ExecuteDrawBatch(context, currentBitmap, m_DestRects, m_Matrices);
        }
    }

    void RendererSystem::ProcessIndividualRender(Registry& registry, Device::Context& context, const std::vector<Entity>& entities)
{
    auto d2d = context.GetD2DContext();
    // ここではアンチエイリアスをONに戻す
    d2d->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    for (auto entity : entities)
    {
        auto& transform = registry.GetComponent<Transform>(entity);
        auto& sprite    = registry.GetComponent<Sprite>(entity);
        if (!sprite.Bitmap) continue;

        if (transform.IsDirty) UpdateSpriteTransform(transform, sprite);

        context.SetTransform(sprite.Transform);
        d2d->DrawBitmap(sprite.Bitmap.get(), D2D1::RectF(0, 0, sprite.Width, sprite.Height));
    }
}

    void RendererSystem::UpdateSpriteTransform(Transform& transform, Sprite& sprite)
    {
        const float cX = sprite.Width * 0.5f;
        const float cY = sprite.Height * 0.5f;
        sprite.Transform = D2D1::Matrix3x2F::Translation(-cX, -cY) * D2D1::Matrix3x2F::Scale(transform.ScaleX, transform.ScaleY) *
                           D2D1::Matrix3x2F::Rotation(transform.Rotation) * D2D1::Matrix3x2F::Translation(transform.X, transform.Y);
        transform.IsDirty = false;
    }

    void RendererSystem::ExecuteDrawBatch(
        Device::Context& context, 
        ID2D1Bitmap* bitmap, 
        const std::vector<D2D1_RECT_F>& rects, 
        const std::vector<D2D1_MATRIX_3X2_F>& matrices)
    {
        auto spriteBatch = context.GetSpriteBatch();
        spriteBatch->Clear();
        spriteBatch->AddSprites(static_cast<UINT32>(rects.size()), rects.data(), nullptr, nullptr, matrices.data());

        context.GetD2DContext3()->DrawSpriteBatch(
            spriteBatch.get(), 
            bitmap, 
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, 
            D2D1_SPRITE_OPTIONS_NONE
        );
    }
}
