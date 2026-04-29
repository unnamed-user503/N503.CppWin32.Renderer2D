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
        // ResourceContainerにリソースを追加し、リソースハンドルを取得する
        auto handle = context.ResourceContainer.Add(Path);

        if (!handle)
        {
            return;
        }

        // リソースハンドルを使用して、リソースコンテナからリソースエントリを取得する
        auto resource = context.ResourceContainer.Get(handle);

        if (!resource)
        {
            return;
        }

        try
        {
            // レジストリにエンティティを作成し、TransformコンポーネントとSpriteコンポーネントを追加する
            auto entity = context.Registry.CreateEntity();

            // Transformコンポーネントを追加する
            auto& transform    = context.Registry.AddComponent(entity, System::Component::Transform{});
            transform.Position.X  = 0.0f;
            transform.Position.Y  = 0.0f;
            transform.Scale.X     = 0.0f;
            transform.Scale.Y     = 0.0f;
            transform.Rotation    = 0.0f;

            // Spriteコンポーネントを追加する
            auto& sprite                  = context.Registry.AddComponent(entity, System::Component::Sprite{ handle });
            sprite.DestinationRect.left   = 0.0f;
            sprite.DestinationRect.top    = 0.0f;
            sprite.DestinationRect.right  = static_cast<float>(resource->Pixels.Width);
            sprite.DestinationRect.bottom = static_cast<float>(resource->Pixels.Height);

            // CreateSpriteパケットのResultにエンティティIDを書き込む
            *Result = entity;
        }
        catch (...)
        {
            context.ResourceContainer.Remove(handle);
        }
    }

} // namespace N503::Renderer2D::Message::Packets
