#pragma once

// 1. Project Headers
#include "CreateResourceCommand.hpp"
#include "DestroyResourceCommand.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Command::Packets
{

    using Packet = std::variant<CreateResourceCommand, DestroyResourceCommand>;

} // namespace N503::Renderer2D::Command::Packets
