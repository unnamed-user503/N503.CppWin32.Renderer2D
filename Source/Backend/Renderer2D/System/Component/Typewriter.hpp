#pragma once

// 1. Project Headers
#include "../../Canvas/Font/Atlas.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <string>

namespace N503::Renderer2D::System::Component
{

    struct Typewriter
    {
        float Speed{ 20.0f };

        float Elapsed{ 0.0f };

        bool IsDirty{ false };

        auto Reset() noexcept -> void
        {
            Speed   = 20.0f;
            Elapsed = 0.0f;
        }
    };

} // namespace N503::Renderer2D::System::Component
