#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <unordered_map>
#include <vector>

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
        struct DrawCommand
        {
            ID2D1Bitmap1* Bitmap{ nullptr };
            D2D1_RECT_F DestinationRect{};
            D2D1_MATRIX_3X2_F Matrix{};
        };

    public:
        RendererSystem();

        auto Update(Registry& registry, Device::Context& context) -> void;

    private:
        std::unordered_map<Entity, D2D1_MATRIX_3X2_F> m_TransformCache;
    };

} // namespace N503::Renderer2D::System
