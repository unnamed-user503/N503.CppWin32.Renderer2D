#include "Pch.hpp"
#include "SpriteBatch.hpp"

// 1. Project Headers
#include "../Engine.hpp"

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

    void SpriteBatch::Execute(ID2D1DeviceContext* context)
    {
        if (Generation != Renderer2D::Engine::Instance().GetTextureRegistry().GetCurrentGeneration())
        {
            Bitmap = Renderer2D::Engine::Instance().GetTextureRegistry().GetOrCreateBitmap(Handle);
            Generation = Renderer2D::Engine::Instance().GetTextureRegistry().GetCurrentGeneration();
        }

        if (Bitmap == nullptr)
        {
            // ビットマップの取得に失敗した場合は描画をスキップ
            return;
        }

        context->DrawBitmap(Bitmap.get(), TargetRect, Opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, SourceRect);
    }

} // namespace N503::Renderer2D::Batch
