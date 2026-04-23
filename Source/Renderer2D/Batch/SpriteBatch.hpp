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

        wil::com_ptr<ID2D1Bitmap1> Bitmap;

        D2D1_RECT_F TargetRect{};

        D2D1_RECT_F SourceRect{};

        float Opacity{ 1.0f };

        std::uint64_t Generation{ 0 };

        auto Execute(ID2D1DeviceContext* context) -> void;
    };

} // namespace N503::Renderer2D::Batch
