#include "Pch.hpp"
#include "Processor.hpp"

// 1. Project Headers
#include "Engine.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>

// 6. C++ Standard Libraries
#include <ranges>
#include <variant>

namespace N503::Renderer2D
{

    auto Processor::AddBatch(const BatchVariant& batch) -> void
    {
        if (m_CurrentBatchCount < MaxBatchSize)
        {
            m_BatchArray[m_CurrentBatchCount++] = batch;
        }
    }

    auto Processor::Process(Device::Context& context) -> bool
    {
        auto d2d = context.GetD2DContext().get();

        // --- ここでスケーリング行列を設定 ---
        // 例: 基準サイズ 1280x720 に対して、現在のウィンドウサイズが 2560x1440 なら 2.0倍
        const auto targetSize = d2d->GetSize(); // 現在のウィンドウ（レンダーターゲット）サイズ
        // const float scaleX    = targetSize.width / 1280.0f; // 1280は仮想横幅
        // const float scaleY    = targetSize.height / 720.0f; // 720は仮想縦幅
        const float scaleX = targetSize.width / 1920.0f;  // 1920は仮想横幅
        const float scaleY = targetSize.height / 1080.0f; // 1080は仮想縦幅

        // 縦横比を維持したい場合は、scaleX と scaleY の小さい方を両方に使う
        // const float scale = std::min(scaleX, scaleY);
        // auto transform = D2D1::Matrix3x2F::Scale(scale, scale);

        auto transform = D2D1::Matrix3x2F::Scale(scaleX, scaleY);
        d2d->SetTransform(transform);
        // ------------------------------------

        // C++20 Views で有効な範囲を走査
        auto activeBatches = m_BatchArray | std::views::take(m_CurrentBatchCount);

        for (auto& variantBatch : activeBatches)
        {
            std::visit(
                [this, d2d](auto&& batch)
                {
                    using T = std::decay_t<decltype(batch)>;

                    if constexpr (std::is_same_v<T, Batch::SpriteBatch>)
                    {
                        batch.Execute(d2d, m_TextureRegistry);
                    }
                    else
                    {
                        //batch.Execute(d2d);
                    }
                },
                variantBatch
            );
        }

        return true;
    }

} // namespace N503::Renderer2D
