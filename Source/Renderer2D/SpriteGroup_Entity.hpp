#pragma once

// 1. Project Headers
#include "System/Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <N503/Renderer2D/SpriteGroup.hpp>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <vector>

namespace N503::Renderer2D
{

    struct SpriteGroup::Entity
    {
        std::vector<System::Entity> ID{};

        std::vector<Transform> Transforms{};
    };

} // namespace N503::Renderer2D
