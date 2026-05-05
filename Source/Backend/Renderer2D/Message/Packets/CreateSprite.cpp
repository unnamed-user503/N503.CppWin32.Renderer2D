#include "Pch.hpp"
#include "CreateSprite.hpp"

// 1. Project Headers
#include "../../Message/Context.hpp"
#include "../../Resource/Container.hpp"
#include "../../System/Entity.hpp"
#include "../../System/Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>

namespace N503::Renderer2D::Message::Packets
{

    auto CreateSprite::operator()(Message::Context& context) const -> void
    {
        if (context.Registry.GetAvailableEntityCount() < 1)
        {
            return;
        }

        auto handle = context.ResourceContainer.Add(Path);

        if (!handle)
        {
            return;
        }

        auto resource = context.ResourceContainer.Get(handle);

        if (!resource)
        {
            return;
        }

        try
        {
            auto entity = context.Registry.CreateEntity();

            auto& sprite = context.Registry.AddComponent(entity, System::Component::Sprite{ handle });
            sprite.SourceRect.left   = SourceRect.Left;
            sprite.SourceRect.top    = SourceRect.Top;
            sprite.SourceRect.right  = SourceRect.Right == 0 ? static_cast<std::uint32_t>(resource->Pixels.Width) : std::min(SourceRect.Right, static_cast<std::uint32_t>(resource->Pixels.Width));
            sprite.SourceRect.bottom = SourceRect.Bottom == 0 ? static_cast<std::uint32_t>(resource->Pixels.Height) : std::min(SourceRect.Bottom, static_cast<std::uint32_t>(resource->Pixels.Height));

            auto& color = context.Registry.AddComponent(entity, System::Component::Color{});

            auto& transform = context.Registry.AddComponent(entity, System::Component::Transform{});

            auto& visible = context.Registry.AddComponent(entity, System::Component::Visible{});

            auto& depth = context.Registry.AddComponent(entity, System::Component::Depth{});

            *Result = entity;
        }
        catch (...)
        {
            context.ResourceContainer.Remove(handle);
        }
    }

} // namespace N503::Renderer2D::Message::Packets
