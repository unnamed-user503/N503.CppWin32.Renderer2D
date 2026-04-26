#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::System
{
    class Registry;
}

namespace N503::Renderer2D::System
{

    class RendererSystem
    {
    public:
        auto Update(Registry& registry, Device::Context& context) -> void;
    };

} // namespace N503::Renderer2D::System
