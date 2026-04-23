#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <cstdint>
#include <string_view>

namespace N503::Renderer2D::Command::Packets
{

    struct CreateResourceCommand final
    {
        std::string_view Path;

        auto operator()() const -> void;
    };

} // namespace N503::Renderer2D::Command::Packets
