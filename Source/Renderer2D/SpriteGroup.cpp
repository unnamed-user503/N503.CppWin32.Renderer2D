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

    SpriteGroup::SpriteGroup(const std::string_view path, std::size_t count)
    {
        m_Entity = std::make_unique<Entity>();
        m_Entity->ID.resize(count);

        std::vector<Message::Packet> packets;
        packets.reserve(count);

        for (std::size_t i = 0; i < count; ++i)
        {
            auto packet = Message::Packets::CreateSprite{
                .Result = &m_Entity->ID[i],
                .Path   = path,
            };

            packets.push_back(std::move(packet));
        }

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packets));
    }

    SpriteGroup::~SpriteGroup()
    {
        if (!m_Entity)
        {
            return;
        }

        std::vector<Message::Packet> packets;
        packets.reserve(m_Entity->ID.size());

        for (std::size_t i = 0; i < m_Entity->ID.size(); ++i)
        {
            auto packet = Message::Packets::DestroyEntity{
                .ID = m_Entity->ID[i],
            };

            packets.push_back(std::move(packet));
        }

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
    }

    auto SpriteGroup::SetTransform(std::function<bool(const std::uint64_t index, Transform&)> delegate) -> void
    {
        if (!m_Entity)
        {
            return;
        }

        std::vector<Message::Packet> packets;
        packets.reserve(m_Entity->ID.size());

        for (std::size_t i = 0; i < m_Entity->ID.size(); ++i)
        {
            Renderer2D::Transform transform{};

            auto isDirty = delegate(i, transform);

            if (!isDirty)
            {
                continue;
            }

            auto packet = Message::Packets::SetTransform{
                .ID        = m_Entity->ID[i],
                .Transform = std::move(transform),
            };

            packets.push_back(std::move(packet));
        }

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packets));
    }

    SpriteGroup::SpriteGroup(SpriteGroup&& other) = default;

    auto SpriteGroup::operator=(SpriteGroup&& other) -> SpriteGroup& = default;

} // namespace N503::Renderer2D
