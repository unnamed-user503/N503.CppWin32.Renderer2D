#pragma once

// 1. Project Headers

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

    struct AddCommandList
    {
        CommandHandle* Result{};

        ResourceHandle ResourceHandle{};

        ResourceType ResourceType{ ResourceType::None };

        std::string_view Text;

        struct
        {
            std::string_view Family;

            float Size{};
        }
        Font;

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
