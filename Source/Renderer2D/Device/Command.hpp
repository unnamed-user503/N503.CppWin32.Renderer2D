#pragma once

// 1. Project Headers
#include "Commands/DrawBitmap.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Device
{

    using namespace Commands;

    using Command = std::variant<std::monostate, DrawBitmap>;

} // namespace N503::Renderer2D::Device
