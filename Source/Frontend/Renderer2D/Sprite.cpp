#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Renderer2D/Engine.hpp"
#include "../../Backend/Renderer2D/Message/Packets/CreateSprite.hpp"
#include "../../Backend/Renderer2D/Message/Packets/DestroyEntity.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetColor.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetRenderGroup.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetTransform.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetVisible.hpp"
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
    N503Sprite N503CreateSprite(const char* path, const N503RectU source)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = std::make_unique<SpriteEntity>();

            auto packet = Message::Packets::CreateSprite{
                .Result     = &entity->EntityID,
                .Path       = path,
                .SourceRect = { source.Left, source.Top, source.Right, source.Bottom },
            };

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

            return reinterpret_cast<N503Sprite>(entity.release());
        }
        CATCH_LOG();

        return nullptr;
    }

    int N503SpriteDestroy(N503Sprite instance)
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

    int N503SetSpriteTransform(N503Sprite instance, const N503Transform2D transform)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetTransform{
                .ID        = entity->EntityID,
                .Transform = {
                    .Position = { transform.Position.X, transform.Position.Y },
                    .Rotation = transform.Rotation,
                    .Scale    = { transform.Scale.X, transform.Scale.Y },
                },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int N503SetSpriteColor(N503Sprite instance, const N503Color color)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetColor{
                .ID    = entity->EntityID,
                .Color = { color.Red, color.Green, color.Blue, color.Alpha },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int N503SetSpriteRenderGroup(N503Sprite instance, uint32_t group)
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

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int N503SetSpriteVisible(N503Sprite instance, uint32_t visible)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<SpriteEntity*>(instance);

            auto packet = Message::Packets::SetVisible{
                .ID        = entity->EntityID,
                .IsEnabled = visible ? true : false,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }
}
