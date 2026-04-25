#pragma once

// 1. Project Headers
#include "Packet.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <semaphore>

namespace N503::Renderer2D::Message
{

    struct Envelope
    {
        Packet Packet;

        std::binary_semaphore* Signal;
    };

} // namespace N503::Renderer2D::Message
