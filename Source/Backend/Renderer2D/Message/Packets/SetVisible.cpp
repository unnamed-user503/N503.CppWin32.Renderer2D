#include "Pch.hpp"
#include "SetVisible.hpp"

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

    auto SetVisible::operator()(Context& context) const -> void
    {
        if (!context.Registry.HasComponent<System::Component::Visible>(ID))
        {
            return;
        }

        auto& visible = context.Registry.GetComponent<System::Component::Visible>(ID);
        visible.IsEnabled = IsEnabled;
    }

} // namespace N503::Renderer2D::Message::Packets
