#include "Pch.hpp"
#include "DestroyEntity.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
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

    auto DestroyEntity::operator()(Context& context) const -> void
    {
    }

} // namespace N503::Renderer2D::Message::Packets
