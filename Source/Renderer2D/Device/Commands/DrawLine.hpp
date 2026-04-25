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

namespace N503::Renderer2D::Device::Commands
{

    struct DrawLine
    {
        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
