#include "Pch.hpp"
#include "CreateSprite.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Message/Context.hpp"
#include "../../Resource/Container.hpp"
#include "../../System/Entity.hpp"
#include "../../System/Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto CreateSprite::operator()(Message::Context& context) const -> void
    {
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

            auto& transform    = context.Registry.AddComponent(entity, System::Transform{});
            transform.X        = 100.0f;
            transform.Y        = 100.0f;
            transform.ScaleX   = 0.3f;
            transform.ScaleY   = 0.3f;
            transform.Rotation = 0.3f;

            auto& sprite  = context.Registry.AddComponent(entity, System::Sprite{ handle });
            sprite.Width  = static_cast<float>(resource->Pixels.Width);
            sprite.Height = static_cast<float>(resource->Pixels.Height);

            *Result = entity;
        }
        catch (...)
        {
            context.ResourceContainer.Remove(handle);
        }
    }

} // namespace N503::Renderer2D::Message::Packets
