#include "Pch.hpp"
#include "DrawBitmap.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Resource/Container.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device::Commands
{

    auto DrawBitmap::Execute(Context& context) -> void
    {
        auto d2dContext = context.GetD2DContext();

        if (!Bitmap)
        {
            auto entry = Engine::GetInstance().GetResourceContainer().Get(ResourceHandle);

            if (!entry)
            {
                return;
            }

            d2dContext->CreateBitmap(
                D2D1::SizeU(entry->Pixels.Width, entry->Pixels.Height),
                entry->Pixels.Bytes,
                entry->Pixels.Pitch,
                D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
                &Bitmap
            );

            if (!Bitmap)
            {
                return;
            }
        }

        d2dContext->DrawBitmap(
            Bitmap.get(),
            D2D1::RectF(0.0f, 0.0f, static_cast<FLOAT>(Bitmap->GetPixelSize().width), static_cast<FLOAT>(Bitmap->GetPixelSize().height)),
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        );
    }

} // namespace N503::Renderer2D::Device::Commands
