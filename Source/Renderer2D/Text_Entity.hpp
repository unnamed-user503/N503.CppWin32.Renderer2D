#pragma once

// 1. Project Headers
#include "Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <N503/Renderer2D/Text.hpp>
#include <N503/Renderer2D/Types.hpp>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D
{

    struct Text::Entity
    {
        CommandHandle CommandHandle{};
    };

} // namespace N503::Renderer2D
