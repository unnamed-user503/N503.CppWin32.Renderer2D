#include "Pch.hpp"
#include "Sprite_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packets/CreateSprite.hpp"
#include "Message/Packets/SetPosition.hpp"

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
        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Sprite::SetPoint(float x, float y, float z) -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::SetPosition{
            .ID = m_Entity->ID,
            .X  = x,
            .Y  = y,
            .Z  = z,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Sprite::SetOpacity(float opacity) -> void
    {
    }

    auto Sprite::GetWidth() const -> float
    {
        return 0.0f;
    }

    auto Sprite::GetHeight() const -> float
    {
        return 0.0f;
    }

    auto Sprite::GetPitch() const -> float
    {
        return 0.0f;
    }

    Sprite::~Sprite() = default;

    Sprite::Sprite(Sprite&& other) = default;

    auto Sprite::operator=(Sprite&& other) -> Sprite& = default;

} // namespace N503::Renderer2D
