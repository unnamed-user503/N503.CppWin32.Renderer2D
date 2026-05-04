#pragma once

namespace N503::Renderer2D::Geometry
{

    struct RectF
    {
        float Left{ 0.0f };

        float Top{ 0.0f };

        float Right{ 0.0f };

        float Bottom{ 0.0f };
    };

    struct RectU
    {
        std::uint32_t Left{ 0 };

        std::uint32_t Top{ 0 };

        std::uint32_t Right{ 0 };

        std::uint32_t Bottom{ 0 };
    };

} // namespace N503::Renderer2D::Geometry
