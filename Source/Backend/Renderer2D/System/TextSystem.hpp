#pragma once

// 1. Project Headers
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Canvas
{
    class Device;
}

namespace N503::Renderer2D::System
{
    class Registry;
}

namespace N503::Renderer2D::System
{

    class TextSystem
    {
    public:
        auto Update(Registry& registry, Canvas::Device& context) -> void;
    };

} // namespace N503::Renderer2D::System
