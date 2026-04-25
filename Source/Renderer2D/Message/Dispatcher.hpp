#pragma once

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message
{

    class Dispatcher final
    {
    public:
        auto Dispatch(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Message
