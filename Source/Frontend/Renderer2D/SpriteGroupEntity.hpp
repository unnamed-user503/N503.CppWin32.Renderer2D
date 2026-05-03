#pragma once

// 1. Project Headers
#include "../../Backend/Renderer2D/System/Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <vector>

namespace N503::Renderer2D
{

    struct SpriteGroupEntity
    {
        std::vector<System::Entity> Entities{};
    };

} // namespace N503::Renderer2D
