#include "Pch.hpp"
#include "Factory.hpp"

// 1. Project Headers
#include "../Engine.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1helper.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Texture
{

    auto Factory::CreateTextureFromHandle(Renderer2D::AssetHandle handle) -> wil::com_ptr<ID2D1Bitmap1>
    {
        // 1. Container から Asset (Pixels::Buffer) を取得
        const auto* asset = Renderer2D::Engine::Instance().GetResourceContainer().GetAsset(handle);

        if (!asset || asset->Pixels.Bytes == nullptr)
        {
            return nullptr;
        }

        const auto& pixels = asset->Pixels;

        // 2. Direct2D ビットマップのプロパティ設定
        // WIC側で 32bppPBGRA に変換済みなので、フォーマットを固定
        const auto pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
        const auto properties  = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, pixelFormat);

        const D2D1_SIZE_U size = D2D1::SizeU(pixels.Width, pixels.Height);

        // 3. GPU へのアップロード（CreateBitmap）
        wil::com_ptr<ID2D1Bitmap1> bitmap;
        const HRESULT hr = Renderer2D::Engine::Instance().GetDeviceContext().GetD2DContext()->CreateBitmap(
            size,
            pixels.Bytes, // CPU側のデータ先頭アドレス (Arena上のメモリ)
            pixels.Pitch, // 1行あたりのバイト数
            &properties,
            &bitmap
        );

        if (FAILED(hr))
        {
            return nullptr;
        }

        return bitmap;
    }

} // namespace N503::Renderer2D
