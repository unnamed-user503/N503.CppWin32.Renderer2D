#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <wincodec.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Batch
{

    struct TextBatch
    {
        auto Execute(ID2D1DeviceContext* context) -> void
        {
        }
    };

} // namespace N503::Renderer2D::Batch
