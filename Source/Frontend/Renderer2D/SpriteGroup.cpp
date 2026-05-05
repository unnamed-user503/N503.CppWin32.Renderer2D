#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Renderer2D/Engine.hpp"
#include "../../Backend/Renderer2D/Message/Packet.hpp"
#include "../../Backend/Renderer2D/Message/Packets/CreateSprite.hpp"
#include "../../Backend/Renderer2D/Message/Packets/DestroyEntity.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetTransform.hpp"
#include "../../Backend/Renderer2D/Message/Queue.hpp"
#include "../../Backend/Renderer2D/System/Entity.hpp"
#include "../../Backend/Renderer2D/System/Registry.hpp"
#include "SpriteGroupEntity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Details/Api.h>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <utility>

extern "C"
{
    n503_renderer2d_sprite_group_h n503_renderer2d_sprite_group_create(const char* path, uint32_t count, const n503_rect_t source)
    {
        using namespace N503::Renderer2D;

        try
        {
            if (Engine::GetInstance().GetSystemRegistry().GetAvailableEntityCount() < count)
            {
                return nullptr;
            }

            auto entity = std::make_unique<SpriteGroupEntity>();

            entity->Entities.resize(count);

            std::vector<Message::Packet> packets;
            packets.reserve(count);

            for (std::size_t i = 0; i < count; ++i)
            {
                packets.emplace_back(Message::Packets::CreateSprite{
                    .Result     = &entity->Entities[i],
                    .Path       = path,
                    .SourceRect = { source.left, source.top, source.right, source.bottom },
                });
            }

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packets));

            return reinterpret_cast<n503_renderer2d_sprite_group_h>(entity.release());
        }
        CATCH_LOG();

        return nullptr;
    }

    int n503_renderer2d_sprite_group_destroy(n503_renderer2d_sprite_group_h instance)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteGroupEntity*>(instance);

            std::vector<Message::Packet> packets;
            packets.reserve(entity->Entities.size());

            for (std::size_t i = 0; i < entity->Entities.size(); ++i)
            {
                packets.emplace_back(Message::Packets::DestroyEntity{
                    .ID = entity->Entities[i],
                });
            }

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));

            delete entity;
            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_group_set_transform(
        n503_renderer2d_sprite_group_h instance, n503_renderer2d_sprite_group_set_transform_delegate_t delegate, void* user_data
    )
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteGroupEntity*>(instance);

            std::vector<Message::Packet> packets;
            packets.reserve(entity->Entities.size());

            for (std::size_t i = 0; i < entity->Entities.size(); ++i)
            {
                n503_transform_t transform{};

                auto isDirty = delegate(i, &transform, user_data);

                if (!isDirty)
                {
                    continue;
                }

                packets.emplace_back(Message::Packets::SetTransform{
                    .ID        = entity->Entities[i],
                    .Transform = {
                        .Position = { .X = transform.position.x, .Y = transform.position.y, .Z = transform.position.z },
                        .Rotation = transform.rotation,
                        .Scale    = { .X = transform.scale.x, .Y = transform.scale.y },
                    },
                });
            }

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_group_set_color(
        n503_renderer2d_sprite_group_h instance, n503_renderer2d_sprite_group_set_color_delegate_t delegate, void* user_data
    )
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteGroupEntity*>(instance);

            std::vector<Message::Packet> packets;
            packets.reserve(entity->Entities.size());

            for (std::size_t i = 0; i < entity->Entities.size(); ++i)
            {
                n503_color_t color{};

                auto isDirty = delegate(i, &color, user_data);

                if (!isDirty)
                {
                    continue;
                }

                packets.emplace_back(Message::Packets::SetColor{
                    .ID    = entity->Entities[i],
                    .Color = { .Red = color.red, .Green = color.green, .Blue = color.blue, .Alpha = color.alpha, },
                });
            }

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_group_set_render_group(n503_renderer2d_sprite_group_h instance, uint32_t group)
    {
        using namespace N503::Renderer2D;

        try
        {
            // TODO;
            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_group_set_visible(n503_renderer2d_sprite_group_h instance, uint32_t visible)
    {
        using namespace N503::Renderer2D;

        try
        {
            // TODO;
            return 0;
        }
        CATCH_LOG();

        return -1;
    }
}
