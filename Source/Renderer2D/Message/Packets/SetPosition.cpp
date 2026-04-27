#include "Pch.hpp"
#include "SetPosition.hpp"

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

    auto SetPosition::operator()(Context& context) const -> void
    {
        if (!context.Registry.HasComponent<System::Transform>(ID))
        {
            return;
        }

        auto& transform = context.Registry.GetComponent<System::Transform>(ID);

        transform.X = X;
        transform.Y = Y;
        transform.Z = Z;
    }

} // namespace N503::Renderer2D::Message::Packets
