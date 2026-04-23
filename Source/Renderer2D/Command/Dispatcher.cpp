#include "Pch.hpp"
#include "Dispatcher.hpp"

// C++ Standard Libraries
#include "Queue.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Command
{

    auto Dispatcher::Dispatch(Queue& commandQueue) -> void
    {
        auto queue = commandQueue.PopAll();

        while (!queue.empty())
        {
            auto& command = queue.front();

            std::visit([](auto&& concrete) { concrete(); }, command.Packet);

            if (command.Signal)
            {
                command.Signal->release();
            }

            queue.pop();
        }
    }

}; // namespace N503::Renderer2D::Command
