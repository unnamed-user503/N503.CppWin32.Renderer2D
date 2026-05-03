#include "Pch.hpp"
#include "SetTransform.hpp"

// 1. Project Headers
#include "../../Message/Context.hpp"
#include "../../System/Entity.hpp"
#include "../../System/Registry.hpp"


// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto SetTransform::operator()(Context& context) const -> void
    {
        if (!context.Registry.HasComponent<System::Component::Transform>(ID))
        {
            return;
        }

        auto& transform = context.Registry.GetComponent<System::Component::Transform>(ID);

        transform.Position.X = Transform.Position.X;
        transform.Position.Y = Transform.Position.Y;
        transform.Position.Z = Transform.Position.Z;
        transform.Rotation   = Transform.Rotation;
        transform.Scale.X    = Transform.Scale.X;
        transform.Scale.Y    = Transform.Scale.Y;
        transform.IsDirty  = true;
    }

} // namespace N503::Renderer2D::Message::Packets
