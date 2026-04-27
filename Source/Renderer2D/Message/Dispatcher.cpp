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

    auto Dispatcher::Dispatch(Queue& queue, Context& context) -> void
    {
        auto bundles = queue.PopAll();

        while (!bundles.empty())
        {
            auto& envelope = bundles.front();

            for (auto& packet : envelope.Packets)
            {
                std::visit([&context](auto&& concrete) { concrete(context); }, packet);
            }

            if (envelope.Signal)
            {
                envelope.Signal->release();
            }

            bundles.pop();
        }
    }

} // namespace N503::Renderer2D::Message
