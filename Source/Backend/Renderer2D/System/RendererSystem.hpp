#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 5. Windows Headers
#include <d2d1_1.h>

// 6. C++ Standard Libraries
#include <array>
#include <vector>

namespace N503::Renderer2D::Canvas
{
    class Session;
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
        RendererSystem() = default;

        ~RendererSystem() = default;

        RendererSystem(const RendererSystem&) = delete;

        auto operator=(const RendererSystem&) -> RendererSystem& = delete;

        auto Update(Registry& registry, Canvas::Session& session) -> void;

    private:
        struct DrawCommand
        {
            ID2D1Bitmap* Bitmap;
            D2D1_RECT_F DestinationRect;
            D2D1_RECT_U SourceRect;
            D2D1_COLOR_F Color;
            D2D1_MATRIX_3X2_F Matrix;
        };

        std::array<D2D1_MATRIX_3X2_F, MaxEntities> m_TransformCache{};
    };

} // namespace N503::Renderer2D::System
