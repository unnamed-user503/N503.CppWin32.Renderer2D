#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <dcommon.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Canvas::Font
{

    struct Glyph
    {
        D2D1_RECT_U SourceRect{ 0 };

        float AdvanceWidth{ 0.0f };

        float BearingX{ 0.0f };

        float BearingY{ 0.0f };
    };

} // namespace N503::Renderer2D::Canvas::Font
