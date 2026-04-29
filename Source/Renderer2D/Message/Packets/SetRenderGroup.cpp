#include "Pch.hpp"
#include "SetRenderGroup.hpp"

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

    auto SetRenderGroup::operator()(Context& context) const -> void
    {
        if (!context.Registry.HasComponent<System::Component::Transform>(ID))
        {
            return;
        }

        auto& sprite = context.Registry.GetComponent<System::Component::Sprite>(ID);
        sprite.Group = RenderGroup;
    }

} // namespace N503::Renderer2D::Message::Packets
