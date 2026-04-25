#include "Pch.hpp"
#include "AddCommandList.hpp"

// 1. Project Headers
#include "../../Device/Command.hpp"
#include "../../Device/CommandList.hpp"
#include "../../Device/Commands/DrawBitmap.hpp"
#include "../../Message/Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto AddCommandList::operator()(Message::Context& context) const -> void
    {
        if (auto handle = context.CommandList.Add(Device::Commands::DrawBitmap{ .ResourceHandle = ResourceHandle }))
        {
            *Result = handle;
        }
    }

} // namespace N503::Renderer2D::Message::Packets
