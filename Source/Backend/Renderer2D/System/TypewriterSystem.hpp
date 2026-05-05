#pragma once

// 1. Project Headers
#include "Entity.hpp"
#include "registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

// Declaration
namespace N503::Renderer2D::System
{

    class TypewriterSystem
    {
    public:
        TypewriterSystem();

        auto Update(Registry& registry, float deltaTime) -> void;

    private:
    };

}
