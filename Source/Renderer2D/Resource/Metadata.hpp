#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Renderer2D::Resource
{

    struct Metadata final
    {
        Renderer2D::Type Type{ Renderer2D::Type::None };

        std::string Path;
    };

} // namespace N503::Renderer2D::Resource
