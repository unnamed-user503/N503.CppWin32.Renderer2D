#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System::Component
{

    struct Depth
    {
        float ZOrderIndex{ 0.0f };

        auto Reset() noexcept -> void
        {
            ZOrderIndex = 0.0f;
        }
    };

} // namespace N503::Renderer2D::System::Component
