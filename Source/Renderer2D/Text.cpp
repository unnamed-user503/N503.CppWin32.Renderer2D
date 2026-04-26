#include "Pch.hpp"
#include "Text_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packets/CreateText.hpp"

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

    Text::Text(const std::string_view text)
    {
        m_Entity = std::make_unique<Entity>();

        auto packet = Message::Packets::CreateText{
            .Result = &m_Entity->ID,
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));
    }

    Text::~Text() = default;

    Text::Text(Text&&) = default;

    auto Text::operator=(Text&&) -> Text& = default;

} // namespace N503::Renderer2D
