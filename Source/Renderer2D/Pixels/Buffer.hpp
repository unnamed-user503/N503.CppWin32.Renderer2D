#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>

namespace N503::Renderer2D::Pixels
{

    struct Buffer
    {
        std::byte* Bytes{ nullptr };

        std::size_t Size{ 0 };

        std::uint32_t Width{ 0 };

        std::uint32_t Height{ 0 };

        std::uint32_t Pitch{ 0 };

        PixelFormat Format{ PixelFormat::Unknown };
    };

} // namespace N503::Renderer2D::Pixels
