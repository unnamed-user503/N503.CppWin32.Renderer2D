#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Metadata.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Resource
{

    struct Asset final
    {
        Renderer2D::AssetHandle Handle;

        Pixels::Buffer Pixels;

        Resource::Metadata Metadata;
    };

} // namespace N503::Renderer2D::Resource
