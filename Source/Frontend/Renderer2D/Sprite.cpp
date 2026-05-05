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
    n503_renderer2d_sprite_h n503_renderer2d_sprite_create(const char* path, const n503_rect_t source)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = std::make_unique<SpriteEntity>();

            auto packet = Message::Packets::CreateSprite{
                .Result     = &entity->EntityID,
                .Path       = path,
                .SourceRect = { source.left, source.top, source.right, source.bottom },
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

    n503_result_t n503_renderer2d_sprite_destroy(n503_renderer2d_sprite_h instance)
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
            return N503_OK;
        }
        CATCH_LOG();

        return N503_ERROR;
    }

    n503_result_t n503_renderer2d_sprite_set_transform(n503_renderer2d_sprite_h instance, const n503_transform_t transform)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetTransform{
                .ID        = entity->EntityID,
                .Transform = { .Position = { transform.position.x, transform.position.y, transform.position.z }, .Rotation = transform.rotation, .Scale = { transform.scale.x, transform.scale.y } },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return N503_OK;
        }
        CATCH_LOG();

        return N503_ERROR;
    }

    n503_result_t n503_renderer2d_sprite_set_color(n503_renderer2d_sprite_h instance, const n503_color_t color)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetColor{
                .ID    = entity->EntityID,
                .Color = { color.red, color.green, color.blue, color.alpha },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return N503_OK;
        }
        CATCH_LOG();

        return N503_ERROR;
    }

    n503_result_t n503_renderer2d_sprite_set_render_group(n503_renderer2d_sprite_h instance, uint32_t group)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetRenderGroup{
                .ID          = entity->EntityID,
                .RenderGroup = static_cast<RenderGroup>(group),
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return N503_OK;
        }
        CATCH_LOG();

        return N503_ERROR;
    }

    n503_result_t n503_renderer2d_sprite_set_visible(n503_renderer2d_sprite_h instance, uint32_t visible)
    {
        using namespace N503::Renderer2D;

        try
        {
            // TODO;
            return N503_OK;
        }
        CATCH_LOG();

        return N503_ERROR;
    }
}
