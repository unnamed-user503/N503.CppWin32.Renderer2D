#pragma once

// 1. Project Headers
#include "Packets/CreateSprite.hpp"
#include "Packets/CreateText.hpp"
#include "Packets/DestroyEntity.hpp"
#include "Packets/SetPosition.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Message
{

    using namespace Packets;

    using Packet = std::variant<CreateSprite, CreateText, DestroyEntity, SetPosition>;

} // namespace N503::Renderer2D::Message
