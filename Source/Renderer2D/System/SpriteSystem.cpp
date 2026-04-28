#include "Pch.hpp"
#include "SpriteSystem.hpp"

// 1. Project Headers
#include "../Device/Context.hpp"
#include "../Resource/Container.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System
{

    auto SpriteSystem::Update(Registry& registry, Device::Context& context, Resource::Container& container) -> void
    {
        for (auto entity : registry.GetView<Sprite>())
        {
            auto& sprite = registry.GetComponent<Sprite>(entity);

            if (sprite.Bitmap)
            {
                continue;
            }

            // GPU側にリソースを作成済みの場合は再利用する
            sprite.Bitmap = context.GetResourceCache().Get(sprite.ResourceHandle);

            if (sprite.Bitmap)
            {
                continue;
            }

            // GPU側にリソースがなければ作成する
            auto resource = container.Get(sprite.ResourceHandle);

            if (!resource)
            {
                continue;
            }

            sprite.Bitmap = context.GetResourceCache().GetOrCreateBitmap(sprite.ResourceHandle, resource->Pixels);

            if (!sprite.Bitmap)
            {
                // ビットマップの作成に失敗した場合は、リソースハンドルを無効化する
                sprite.ResourceHandle = { .ID = Handle::ResourceID::Invalid, .Type = Handle::ResourceType::None, .Generation = Handle::Generation::Default };
            }
        }
    }

} // namespace N503::Renderer2D::System
