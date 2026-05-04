#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Renderer2D/Engine.hpp"
#include "../../Backend/Renderer2D/Message/Packets/DestroyEntity.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetTransform.hpp"
#include "../../Backend/Renderer2D/Message/Queue.hpp"
#include "TextEntity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Details/Api.h>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <utility>

extern "C"
{
    n503_renderer2d_text_h n503_renderer2d_text_create(const char* text, const char* font, float size, float r, float g, float b, float a)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = std::make_unique<TextEntity>();

            auto packet = Message::Packets::CreateText{
                .Result   = &entity->EntityID,
                .Text     = text,
                .FontName = font,
                .FontSize = size,
                .Color    = { r, g, b, a },
            };

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

            if (!entity->EntityID)
            {
                throw std::runtime_error("text creation failed.");
            }

            return reinterpret_cast<n503_renderer2d_text_h>(entity.release());
        }
        CATCH_LOG();

        return nullptr;
    }

    int n503_renderer2d_text_destroy(n503_renderer2d_text_h instance)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);

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

    int n503_renderer2d_text_set_transform(n503_renderer2d_text_h instance, float x, float y, float scaleX, float scaleY, float rotation)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);

            auto packet = Message::Packets::SetTransform{
                .ID        = entity->EntityID,
                .Transform = { .Position = { x, y, 0.0f }, .Rotation = rotation, .Scale = { scaleX, scaleY } },
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    int n503_renderer2d_text_set_color(n503_renderer2d_text_h instance, float r, float g, float b, float a)
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

    int n503_renderer2d_text_set_visible(n503_renderer2d_text_h instance, uint32_t visible)
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
