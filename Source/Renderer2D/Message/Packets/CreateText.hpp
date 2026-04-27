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

    struct CreateText
    {
        System::Entity* Result{ nullptr };

        std::string Text{};

        std::string FontName{};

        float FontSize{ 0.0f };

        ColorF Color{ 1.0f, 1.0f, 1.0f, 1.0f };

        auto operator()(Message::Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
