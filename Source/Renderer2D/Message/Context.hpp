#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    class Queue;
}

namespace N503::Renderer2D::Device
{
    class Context;
    class CommandList;
} // namespace N503::Renderer2D::Device

namespace N503::Renderer2D::Resource
{
    class Container;
}

namespace N503::Renderer2D::Message
{

    struct Context
    {
        Message::Queue& MessageQueue;

        Device::Context& DeviceContext;

        Device::CommandList& CommandList;

        Resource::Container& ResourceContainer;
    };

} // namespace N503::Renderer2D::Message
