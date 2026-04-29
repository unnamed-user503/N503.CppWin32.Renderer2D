#include "Pch.hpp"
#include "SpriteGroup_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packet.hpp"
#include "Message/Packets/CreateSprite.hpp"
#include "Message/Packets/DestroyEntity.hpp"
#include "Message/Packets/SetTransform.hpp"
#include "Message/Queue.hpp"

#include "System/Entity.hpp"
#include "System/Registry.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/SpriteGroup.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>
#include <utility>

namespace N503::Renderer2D
{

    SpriteGroup::SpriteGroup(const std::string_view path, std::size_t count, const Geometry::RectU sourceRect)
    {
        if (Engine::GetInstance().GetSystemRegistry().GetAvailableEntityCount() < count)
        {
            return;
        }

        m_Entity = std::make_unique<Entity>();

        // 事前にIDのサイズを確保しておくことで、CreateSpriteパケットのResultに直接IDを書き込めるようにする
        m_Entity->ID.resize(count);

        // CreateSpriteパケットを一括で送信するための準備
        std::vector<Message::Packet> packets;
        packets.reserve(count);

        // CreateSpriteパケットを作成し、ResultにIDの書き込み先を指定する
        for (std::size_t i = 0; i < count; ++i)
        {
            auto packet = Message::Packets::CreateSprite{
                .Result     = &m_Entity->ID[i],
                .Path       = path,
                .SourceRect = sourceRect
            };

            packets.push_back(std::move(packet));
        }

        // パケットを送信する前にエンジンが起動していない場合は起動する
        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packets));
    }

    SpriteGroup::~SpriteGroup()
    {
        if (!m_Entity)
        {
            return;
        }

        // DestroyEntityパケットを一括で送信するための準備
        std::vector<Message::Packet> packets;
        packets.reserve(m_Entity->ID.size());

        // DestroyEntityパケットを作成する
        for (std::size_t i = 0; i < m_Entity->ID.size(); ++i)
        {
            auto packet = Message::Packets::DestroyEntity{
                .ID = m_Entity->ID[i],
            };

            packets.push_back(std::move(packet));
        }

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
    }

    auto SpriteGroup::SetTransform(std::function<bool(const std::uint64_t, Geometry::Transform&)> delegate) -> void
    {
        if (!m_Entity)
        {
            return;
        }

        // Transformの変更が必要なスプライトに対してのみ、SetTransformパケットを作成して送信するための準備
        std::vector<Message::Packet> packets;
        packets.reserve(m_Entity->ID.size());

        for (std::size_t i = 0; i < m_Entity->ID.size(); ++i)
        {
            Geometry::Transform transform{};

            // デリゲートを呼び出して、Transformの変更が必要かどうかを判断する
            auto isDirty = delegate(i, transform);

            if (!isDirty)
            {
                continue;
            }

            // Transformが変更された場合のみ、SetTransformパケットを作成して送信する
            auto packet = Message::Packets::SetTransform{
                .ID        = m_Entity->ID[i],
                .Transform = std::move(transform),
            };

            packets.push_back(std::move(packet));
        }

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
    }

    auto SpriteGroup::SetColor(std::function<bool(const std::uint64_t, ColorF&)> delegate) -> void
    {
        if (!m_Entity)
        {
            return;
        }

        // Transformの変更が必要なスプライトに対してのみ、SetTransformパケットを作成して送信するための準備
        std::vector<Message::Packet> packets;
        packets.reserve(m_Entity->ID.size());

        for (std::size_t i = 0; i < m_Entity->ID.size(); ++i)
        {
            ColorF color{};

            // デリゲートを呼び出して、Transformの変更が必要かどうかを判断する
            auto isDirty = delegate(i, color);

            if (!isDirty)
            {
                continue;
            }

            // Transformが変更された場合のみ、SetTransformパケットを作成して送信する
            auto packet = Message::Packets::SetColor{
                .ID    = m_Entity->ID[i],
                .Color = std::move(color),
            };

            packets.push_back(std::move(packet));
        }

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
    }

    SpriteGroup::SpriteGroup(SpriteGroup&& other) = default;

    auto SpriteGroup::operator=(SpriteGroup&& other) -> SpriteGroup& = default;

} // namespace N503::Renderer2D
