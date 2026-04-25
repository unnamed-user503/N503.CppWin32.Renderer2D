#include "Pch.hpp"
#include "Text_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packets/AddCommandList.hpp"
#include "Message/Packets/CreateBitmap.hpp"
#include "Message/Packets/UpdateBitmapOpacity.hpp"
#include "Message/Packets/UpdateBitmapPoint.hpp"

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

        auto packet = Message::Packets::AddCommandList{
            .Result       = &m_Entity->CommandHandle,
            .ResourceType = ResourceType::String,
            .Text = text,
            .Font = {
                .Family = "Segoe UI",
                .Size   = 24.0f,
            },
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    Text::~Text() = default;

    Text::Text(Text&&) = default;

    auto Text::operator=(Text&&) -> Text& = default;

} // namespace N503::Renderer2D
