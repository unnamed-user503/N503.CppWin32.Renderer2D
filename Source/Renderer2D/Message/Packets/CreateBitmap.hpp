#pragma once

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Pixels/Buffer.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string_view>

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct CreateBitmap
    {
        ResourceHandle* Result{ nullptr };

        Pixels::Buffer* Pixels{ nullptr };

        std::string_view Path{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
