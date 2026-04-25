#include "Pch.hpp"
#include "AddCommandList.hpp"

// 1. Project Headers
#include "../../Device/Command.hpp"
#include "../../Device/CommandList.hpp"
#include "../../Device/Commands/DrawBitmap.hpp"
#include "../../Device/Commands/DrawString.hpp"
#include "../../Device/Commands/DrawTextLayout.hpp"
#include "../../Message/Context.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <utility>
#include <string>

namespace N503::Renderer2D::Message::Packets
{

    auto AddCommandList::operator()(Message::Context& context) const -> void
    {
        switch (ResourceType)
        {
            case ResourceType::Bitmap:
                if (auto handle = context.CommandList.Add(Device::Commands::DrawBitmap{ .ResourceHandle = ResourceHandle }))
                {
                    *Result = handle;
                }
                break;

            case ResourceType::String:
                auto command = Device::Commands::DrawTextLayout{
                    .Text        = std::string(Text),
                    .Destination = { 0.0f, 0.0f },
                    .Font = {
                        .Family = std::string(Font.Family),
                        .Size   = Font.Size,
                    },
                };

                if (auto handle = context.CommandList.Add(std::move(command)))
                {
                    *Result = handle;
                }

                break;
        }
    }

} // namespace N503::Renderer2D::Message::Packets
