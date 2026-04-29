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

namespace N503::Renderer2D::System::Component
{

    struct Sprite
    {
        Renderer2D::ResourceHandle ResourceHandle;

        wil::com_ptr<ID2D1Bitmap1> Bitmap;

        RenderGroup Group{ RenderGroup::Normal };

        D2D1_RECT_F DestinationRect{};

        D2D1_RECT_F SourceRect{};

        auto Reset() noexcept -> void
        {
            ResourceHandle = { .ID = Handle::ResourceID::Invalid, .Type = Handle::ResourceType::None, .Generation = Handle::Generation::Default };
            Bitmap.reset();
            Group           = RenderGroup::Normal;
            DestinationRect = {};
            SourceRect      = {};
        }
    };

}
