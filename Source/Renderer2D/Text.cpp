#include "Pch.hpp"
#include "Text_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packet.hpp"
#include "Message/Packets/CreateSprite.hpp"
#include "Message/Packets/DestroyEntity.hpp"
#include "Message/Packets/SetTransform.hpp"
#include "Message/Queue.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Text.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace N503::Renderer2D
{

    Text::Text(const std::string_view text, const std::string_view font, const float size, ColorF color)
    {
        m_Entity = std::make_unique<Entity>();

        auto packet = Message::Packets::CreateText{
            .Result   = &m_Entity->ID,
            .Text     = std::string{ text },
            .FontName = std::string{ font },
            .FontSize = size,
            .Color    = color,
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));
    }

    Text::~Text()
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

    auto Text::SetTransform(const Geometry::Transform& transform) -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::SetTransform{
            .ID        = m_Entity->ID,
            .Transform = transform,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    Text::Text(Text&&) = default;

    auto Text::operator=(Text&&) -> Text& = default;

} // namespace N503::Renderer2D
