#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Renderer2D/Engine.hpp"
#include "../../Backend/Renderer2D/Message/Packet.hpp"
#include "../../Backend/Renderer2D/Message/Packets/CreateSprite.hpp"
#include "../../Backend/Renderer2D/Message/Packets/DestroyEntity.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetColor.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetRenderGroup.hpp"
#include "../../Backend/Renderer2D/Message/Packets/SetTransform.hpp"
#include "../../Backend/Renderer2D/Message/Queue.hpp"
#include "../../Backend/Renderer2D/System/Entity.hpp"
#include "../../Backend/Renderer2D/System/Registry.hpp"
#include "SpriteGroupEntity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Details/Api.h>

// 6. C++ Standard Libraries
#include <memory>
#include <utility>
#include <vector>

extern "C"
{
    // スプライトグループの生成[cite: 3, 4]
    N503SpriteGroup N503CreateSpriteGroup(const char* path, uint16_t count, const N503RectU source)
    {
        using namespace N503::Renderer2D;

        try
        {
            // 利用可能なエンティティ枠の確認[cite: 4]
            if (Engine::GetInstance().GetSystemRegistry().GetAvailableEntityCount() < count)
            {
                return nullptr;
            }

            auto entity = std::make_unique<SpriteGroupEntity>();
            entity->Entities.resize(count);

            std::vector<Message::Packet> packets;
            packets.reserve(count);

            // 個別スプライトエンティティ生成パケットの構築[cite: 4]
            for (uint32_t i = 0; i < count; ++i)
            {
                packets.emplace_back(Message::Packets::CreateSprite{
                    .Result     = &entity->Entities[i],
                    .Path       = path,
                    .SourceRect = { source.Left, source.Top, source.Right, source.Bottom },
                });
            }

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packets));

            return reinterpret_cast<N503SpriteGroup>(entity.release());
        }
        CATCH_LOG();

        return nullptr;
    }

    // スプライトグループの破棄[cite: 3, 4]
    int N503DestroySpriteGroup(N503SpriteGroup instance)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto group = reinterpret_cast<SpriteGroupEntity*>(instance);
            if (!group) return -1;

            std::vector<Message::Packet> packets;
            packets.reserve(group->Entities.size());

            for (auto entityID : group->Entities)
            {
                packets.emplace_back(Message::Packets::DestroyEntity{
                    .ID = entityID,
                });
            }

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));

            delete group;
            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    // Transformの一括更新（Batch）[cite: 3, 4]
    int N503SetSpriteGroupTransformBatch(N503SpriteGroup instance, N503SetSpriteGroupTransformDelegate delegate, void* user_data)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto group = reinterpret_cast<SpriteGroupEntity*>(instance);
            if (!group || !delegate) return -1;

            std::vector<Message::Packet> packets;
            packets.reserve(group->Entities.size());

            for (uint32_t i = 0; i < static_cast<uint32_t>(group->Entities.size()); ++i)
            {
                N503Transform2D transform{};
                // デリゲート経由でユーザーから値を取得。戻り値が1(Dirty)ならパケット生成[cite: 1, 4]
                if (delegate(i, &transform, user_data))
                {
                    packets.emplace_back(Message::Packets::SetTransform{
                        .ID        = group->Entities[i],
                        .Transform = {
                            .Position = { transform.Position.X, transform.Position.Y },
                            .Rotation = transform.Rotation,
                            .Scale    = { transform.Scale.X, transform.Scale.Y },
                        },
                    });
                }
            }

            if (!packets.empty())
            {
                Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
            }

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    // Colorの一括更新（Batch）[cite: 3, 4]
    int N503SetSpriteGroupColorBatch(N503SpriteGroup instance, N503SetSpriteGroupColorDelegate delegate, void* user_data)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto group = reinterpret_cast<SpriteGroupEntity*>(instance);
            if (!group || !delegate) return -1;

            std::vector<Message::Packet> packets;
            packets.reserve(group->Entities.size());

            for (uint32_t i = 0; i < static_cast<uint32_t>(group->Entities.size()); ++i)
            {
                N503Color color{};
                if (delegate(i, &color, user_data))
                {
                    packets.emplace_back(Message::Packets::SetColor{
                        .ID    = group->Entities[i],
                        .Color = { color.Red, color.Green, color.Blue, color.Alpha },
                    });
                }
            }

            if (!packets.empty())
            {
                Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
            }

            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    // 可視性の一括更新（Batch）[cite: 3, 4]
    int N503SetSpriteGroupVisibleBatch(N503SpriteGroup instance, N503SetSpriteGroupVisibleDelegate delegate, void* user_data)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto group = reinterpret_cast<SpriteGroupEntity*>(instance);
            if (!group || !delegate) return -1;

            // TODO: BackendにSetVisibleパケットがある前提で実装
            // 現状のSprite.cpp(source 5)でもTODOのため、構造のみ提供[cite: 5]
            return 0;
        }
        CATCH_LOG();

        return -1;
    }

    // 描画グループ（レイヤー）の一括更新[cite: 3, 5]
    int N503SetSpriteGroupRenderGroupBatch(N503SpriteGroup instance, uint32_t group_id)
    {
        using namespace N503::Renderer2D;

        try
        {
            auto group = reinterpret_cast<SpriteGroupEntity*>(instance);
            if (!group) return -1;

            std::vector<Message::Packet> packets;
            packets.reserve(group->Entities.size());

            for (auto entityID : group->Entities)
            {
                packets.emplace_back(Message::Packets::SetRenderGroup{
                    .ID          = entityID,
                    .RenderGroup = static_cast<RenderGroup>(group_id),
                });
            }

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));

            return 0;
        }
        CATCH_LOG();

        return -1;
    }
}
