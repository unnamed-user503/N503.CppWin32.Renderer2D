#pragma once

// 1. Project Headers
#include "System/Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <N503/Renderer2D/Bitmap.hpp>
#include <N503/Renderer2D/Types.hpp>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D
{

    struct Bitmap::Entity
    {
        System::Entity ID{};
    };

} // namespace N503::Renderer2D
