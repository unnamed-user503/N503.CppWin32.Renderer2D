#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Command::Packets
{

    struct DestroyResourceCommand final
    {
        auto operator()() const -> void;
    };

} // namespace N503::Renderer2D::Command::Packets
