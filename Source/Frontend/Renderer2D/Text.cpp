#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Renderer2D/Engine.hpp"
#include "../../Backend/Renderer2D/Message/Packets/CreateText.hpp" // CreateTextパケットが必要
#include "../../Backend/Renderer2D/Message/Packets/DestroyEntity.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetColor.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetContent.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetTransform.hpp"
#include "../../Backend/Renderer2D/Message/Queue.hpp"
#include "TextEntity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Details/Api.h>

extern "C"
{
    N503Text N503CreateText(const char* content, const char* font, float size)
    {
        using namespace N503::Renderer2D;
        try
        {
            auto entity = std::make_unique<TextEntity>();
            auto packet = Message::Packets::CreateText{
                .Result = &entity->EntityID, .Text = content, .FontName = font, .FontSize = size,
                // 色は初期値として白を設定するか、パケット側で対応
            };

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

            return reinterpret_cast<N503Text>(entity.release());
        }
        CATCH_LOG();
        return nullptr;
    }

    int N503DestroyText(N503Text instance)
    {
        using namespace N503::Renderer2D;
        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);
            auto packet = Message::Packets::DestroyEntity{ .ID = entity->EntityID };
            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
            delete entity;
            return 0;
        }
        CATCH_LOG();
        return -1;
    }

    int N503SetTextTransform(N503Text instance, const N503Transform2D transform)
    {
        using namespace N503::Renderer2D;
        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);
            auto packet = Message::Packets::SetTransform{ .ID = entity->EntityID, .Transform = { .Position = { transform.Position.X, transform.Position.Y }, .Rotation = transform.Rotation, .Scale = { transform.Scale.X, transform.Scale.Y } } };
            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
            return 0;
        }
        CATCH_LOG();
        return -1;
    }

    int N503SetTextColor(N503Text instance, const N503Color color)
    {
        using namespace N503::Renderer2D;
        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);
            auto packet = Message::Packets::SetColor{ .ID = entity->EntityID, .Color = { color.Red, color.Green, color.Blue, color.Alpha } };
            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
            return 0;
        }
        CATCH_LOG();
        return -1;
    }

    int N503SetTextVisible(N503Text instance, uint32_t visible)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);

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

    int N503SetTextContent(N503Text instance, const char* content)
    {
        using namespace N503::Renderer2D;
        try
        {
            auto entity = reinterpret_cast<TextEntity*>(instance);
            auto packet = Message::Packets::SetContent{ .ID = entity->EntityID, .Content = std::string(content) };
            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
            return 0;
        }
        CATCH_LOG();
        return -1;
    }
}
