#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

#undef DrawText

namespace N503::Renderer2D::Device::Commands
{
    struct DrawText
    {
        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
