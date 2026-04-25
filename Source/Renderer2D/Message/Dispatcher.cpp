#include "Pch.hpp"
#include "Dispatcher.hpp"

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Context.hpp"
#include "Envelope.hpp"
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Message
{

    auto Dispatcher::Dispatch(Context& context) -> void
    {
        auto envelopes = context.MessageQueue.PopAll();

        while (!envelopes.empty())
        {
            auto& message = envelopes.front();

            auto cleanup = wil::scope_exit(
                [&message]()
                {
                    if (message.Signal)
                    {
                        message.Signal->release();
                    }
                }
            );

            std::visit([&context](auto&& concrete) { concrete(context); }, message.Packet);

            envelopes.pop();
        }
    }

} // namespace N503::Renderer2D::Message
