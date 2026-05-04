#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Resource
{
    class Container;
}

namespace N503::Renderer2D::System
{
    class Registry;
}

namespace N503::Renderer2D::Canvas
{
    class Device;
}

namespace N503::Renderer2D::Message
{

    struct Context
    {
        Resource::Container& ResourceContainer;

        System::Registry& Registry;

        Canvas::Device& CanvasDevice;
    };

} // namespace N503::Renderer2D::Message
