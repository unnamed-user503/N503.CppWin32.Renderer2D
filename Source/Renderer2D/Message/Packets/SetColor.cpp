#include "Pch.hpp"
#include "SetColor.hpp"

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

    auto SetColor::operator()(Context& context) const -> void
    {
        if (!context.Registry.HasComponent<System::Component::Transform>(ID))
        {
            return;
        }

        auto& color = context.Registry.GetComponent<System::Component::Color>(ID);
        color.Red   = Color.Red;
        color.Green = Color.Green;
        color.Blue  = Color.Blue;
        color.Alpha = Color.Alpha;
    }

} // namespace N503::Renderer2D::Message::Packets
