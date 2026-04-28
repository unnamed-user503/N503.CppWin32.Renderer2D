#include "Pch.hpp"
#include "SetTransform.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Message/Context.hpp"
#include "../../Resource/Container.hpp"
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
        if (!context.Registry.HasComponent<System::Transform>(ID))
        {
            return;
        }

        auto& transform = context.Registry.GetComponent<System::Transform>(ID);

        transform.X        = Transform.Position.X;
        transform.Y        = Transform.Position.Y;
        transform.Z        = Transform.Position.Z;
        transform.Rotation = Transform.Rotation;
        transform.ScaleX   = Transform.Scale.X;
        transform.ScaleY   = Transform.Scale.Y;
        transform.IsDirty  = true;
    }

} // namespace N503::Renderer2D::Message::Packets
