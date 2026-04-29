#pragma once

// 1. Project Headers
#include "../../System/Entity.hpp"

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

    struct CreateSprite
    {
        System::Entity* Result{ nullptr };

        std::string_view Path{};

        Geometry::RectU SourceRect{};

        auto operator()(Message::Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
