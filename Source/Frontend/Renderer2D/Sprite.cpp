#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Renderer2D/Engine.hpp"
#include "../../Backend/Renderer2D/Message/Packets/CreateSprite.hpp"
#include "../../Backend/Renderer2D/Message/Packets/DestroyEntity.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetColor.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetRenderGroup.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetTransform.hpp"
#include "../../Backend/Renderer2D/Message/Queue.hpp"
#include "SpriteEntity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Details/Api.h>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <utility>


extern "C"
{

    n503_renderer2d_sprite_h n503_renderer2d_sprite_create(const char* path, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = std::make_unique<SpriteEntity>();

            auto packet = Message::Packets::CreateSprite{
                .Result     = &entity->EntityID,
                .Path       = path,
                .SourceRect = { left, top, right, bottom },
            };

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));
            /*
            if (!entity->EntityID)
            {
                throw std::runtime_error("sprite creation failed.");
            }
            */
            return reinterpret_cast<n503_renderer2d_sprite_h>(entity.release());
        }
        CATCH_LOG();

        return nullptr;
    }

    int n503_renderer2d_sprite_destroy(n503_renderer2d_sprite_h instance)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::DestroyEntity{
                .ID = entity->EntityID,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            delete entity;
            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_set_transform(n503_renderer2d_sprite_h instance, float x, float y, float scaleX, float scaleY, float rotation)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetTransform{
                .ID = entity->EntityID,
                .Transform = { x, y, rotation, scaleX, scaleY },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_set_color(n503_renderer2d_sprite_h instance, float r, float g, float b, float a)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetColor{
                .ID = entity->EntityID,
                .Color = { r, g, b, a },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_set_render_group(n503_renderer2d_sprite_h instance, uint32_t group)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetRenderGroup{
                .ID = entity->EntityID,
                .RenderGroup = static_cast<RenderGroup>(group),
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_sprite_set_visible(n503_renderer2d_sprite_h instance, uint32_t visible)
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
