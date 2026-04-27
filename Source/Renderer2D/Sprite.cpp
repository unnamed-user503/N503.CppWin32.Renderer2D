#include "Pch.hpp"
#include "Sprite_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Queue.hpp"
#include "Message/Packets/CreateSprite.hpp"
#include "Message/Packets/DestroyEntity.hpp"
#include "Message/Packets/SetTransform.hpp"

#include "System/Entity.hpp"
#include "System/Registry.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Sprite.hpp>
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

    Sprite::Sprite(const std::string_view path)
    {
        m_Entity = std::make_unique<Entity>();

        auto packet = Message::Packets::CreateSprite{
            .Result = &m_Entity->ID,
            .Path   = path,
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));
    }
    
    Sprite::~Sprite()
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::DestroyEntity{
            .ID = m_Entity->ID,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Sprite::SetTransform(const Transform & transform) -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::SetTransform{
            .ID = m_Entity->ID,
            .Transform = transform,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Sprite::SetOpacity(float opacity) -> void
    {
    }

    Sprite::Sprite(Sprite&& other) = default;

    auto Sprite::operator=(Sprite&& other) -> Sprite& = default;

} // namespace N503::Renderer2D
