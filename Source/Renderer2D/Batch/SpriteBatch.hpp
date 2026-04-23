#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Batch
{

    struct SpriteBatch
    {
        Renderer2D::AssetHandle Handle;

        D2D1_RECT_F TargetRect;

        D2D1_RECT_F SourceRect;

        float Opacity{ 1.0f };

        void Execute(ID2D1DeviceContext* context, ID2D1Bitmap1* bitmap) const
        {
            if (!bitmap)
            {
                return; // Bitmapがない場合は描画できないので早期リターン
            }

            context->DrawBitmap(bitmap, TargetRect, Opacity, D2D1_INTERPOLATION_MODE_LINEAR, SourceRect);
        }
    };

} // namespace N503::Renderer2D::Batch
