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
        const auto targetSize = d2d->GetSize();             // 現在のウィンドウ（レンダーターゲット）サイズ
        const float scaleX    = targetSize.width / 1280.0f; // 1280は仮想横幅
        const float scaleY    = targetSize.height / 720.0f; // 720は仮想縦幅

        // 縦横比を維持したい場合は、scaleX と scaleY の小さい方を両方に使う
        // const float scale = std::min(scaleX, scaleY);
        // auto transform = D2D1::Matrix3x2F::Scale(scale, scale);

        auto transform = D2D1::Matrix3x2F::Scale(scaleX, scaleY);
        d2d->SetTransform(transform);
        // ------------------------------------

        // C++20 Views で有効な範囲を走査
        auto activeBatches = m_BatchArray | std::views::take(m_CurrentBatchCount);

        for (const auto& batch : activeBatches)
        {
            std::visit(
                [this, d2d](auto&& arg)
                {
                    using T = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<T, Batch::SpriteBatch>)
                    {
                        // SpriteBatch の場合のみ、Handle から Bitmap を引いて渡す
                        auto bitmap = GetOrCreateBitmap(arg.Handle);
                        arg.Execute(d2d, bitmap.get());
                    }
                    else
                    {
                        // 他の図形バッチなどは Bitmap 不要で実行
                        // arg.Execute(d2d, nullptr);
                    }
                },
                batch
            );
        }

        return true;
    }

    // 内部的なBitmapキャッシュ管理
    auto Processor::GetOrCreateBitmap(AssetHandle handle) -> wil::com_ptr<ID2D1Bitmap1>
    {
        // 1. キャッシュヒット確認 (最速パス)
        if (auto it = m_BitmapMap.find(handle); it != m_BitmapMap.end())
        {
            return it->second;
        }

        // 2. Container から Asset (Pixels::Buffer) を取得
        // ※ m_Container はコンストラクタで保持している前提
        const auto* asset = Renderer2D::Engine::Instance().GetResourceContainer().GetAsset(handle);
        if (!asset || asset->Pixels.Bytes == nullptr)
        {
            return nullptr;
        }

        const auto& pixels = asset->Pixels;

        // 3. Direct2D ビットマップのプロパティ設定
        // WIC側で 32bppPBGRA に変換済みなので、フォーマットを固定
        const D2D1_BITMAP_PROPERTIES1
            props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        const D2D1_SIZE_U size = D2D1::SizeU(pixels.Width, pixels.Height);

        // 4. GPU へのアップロード（CreateBitmap）
        // Audio の SourceReader から Voice へ流し込む処理に相当
        wil::com_ptr<ID2D1Bitmap1> bitmap;
        const HRESULT hr = Renderer2D::Engine::Instance().GetDeviceContext().GetD2DContext()->CreateBitmap(
            size,
            pixels.Bytes, // CPU側のデータ先頭アドレス (Arena上のメモリ)
            pixels.Pitch, // 1行あたりのバイト数
            &props,
            &bitmap
        );

        if (FAILED(hr))
        {
            // ログ出力などのエラーハンドリング
            return nullptr;
        }

        // 5. キャッシュに登録して返却
        m_BitmapMap[handle] = bitmap;
        return bitmap;
    }

} // namespace N503::Renderer2D
