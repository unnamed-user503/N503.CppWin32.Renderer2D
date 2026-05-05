#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System::Component
{

    struct Transform
    {
        Geometry::Vector2F Position{ 0.0f, 0.0f };

        float Rotation{ 0.0f };

        Geometry::Vector2F Scale{ 1.0f, 1.0f };

        bool IsDirty{ false };

        auto Reset() noexcept -> void
        {
            Position.X = 0.0f;
            Position.Y = 0.0f;

            Rotation = 0.0f;

            Scale.X = 0.0f;
            Scale.Y = 0.0f;

            IsDirty = false;
        }
    };

} // namespace N503::Renderer2D::System::Component
