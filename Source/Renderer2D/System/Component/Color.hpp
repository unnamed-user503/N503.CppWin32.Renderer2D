#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System::Component
{

    struct Color
    {
        float Red{ 1.0f };

        float Green{ 1.0f };

        float Blue{ 1.0f };

        float Alpha{ 1.0f };

        auto Reset() noexcept -> void
        {
            Red     = 1.0f;
            Green   = 1.0f;
            Blue    = 1.0f;
            Alpha   = 1.0f;
        }
    };

}
