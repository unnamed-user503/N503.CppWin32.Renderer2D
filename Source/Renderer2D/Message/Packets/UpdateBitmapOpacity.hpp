#pragma once

// 1. Project Headers
#include "../../Device/Context.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct UpdateBitmapOpacity
    {
        CommandHandle CommandHandle{};

        float Opacity{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
