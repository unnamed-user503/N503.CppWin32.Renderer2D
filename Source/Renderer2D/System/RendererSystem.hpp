#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <vector>

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::System
{
    class Registry;
}

namespace N503::Renderer2D::System
{

    class RendererSystem
    {
    public:
        RendererSystem() = default;
        ~RendererSystem() = default;

        // メイン更新関数
        auto Update(Registry& registry, Device::Context& context) -> void;

    private:
        // 1. 各レイヤーを適切なルールで並べ替える
        void SortLayer(Registry& registry, std::vector<Entity>& entities, RenderGroup group);

        // 2. エンティティリストを走査し、同じビットマップごとにバッチを切り出す
        void ProcessBatchRender(Registry& registry, Device::Context& context, const std::vector<Entity>& entities);

        void ProcessIndividualRender(Registry& registry, Device::Context& context, const std::vector<Entity>& entities);

        // 3. 実際に D2D の描画コマンドを発行する（副作用をこの関数に限定）
        void ExecuteDrawBatch(
            Device::Context& context,
            ID2D1Bitmap* bitmap,
            const std::vector<D2D1_RECT_F>& rects,
            const std::vector<D2D1_MATRIX_3X2_F>& matrices
        );

        // 4. Transformコンポーネントから行列を再計算する
        void UpdateSpriteTransform(Transform& transform, Sprite& sprite);

    private:
        // --- メモリ再確保を避けるためのキャッシュ用メンバ変数 ---
        
        // レイヤー仕分け用バケツ
        std::array<std::vector<Entity>, (size_t)RenderGroup::Limited> m_Buckets;

        // SpriteBatchに流し込むための一時バッファ
        std::vector<D2D1_RECT_F> m_DestRects;
        std::vector<D2D1_MATRIX_3X2_F> m_Matrices;
    };

} // namespace N503::Renderer2D::System
