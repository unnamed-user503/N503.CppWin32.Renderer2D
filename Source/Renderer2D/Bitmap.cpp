#include "Pch.hpp"
#include "Bitmap_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packets/CreateSprite.hpp"
#include "Message/Packets/SetPosition.hpp"

#include "System/Entity.hpp"
#include "System/Registry.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Bitmap.hpp>
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

    Bitmap::Bitmap(const std::string_view path)
    {
        m_Entity = std::make_unique<Entity>();

        auto packet = Message::Packets::CreateSprite{
            .Result = &m_Entity->ID,
            .Path   = path,
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));
    }

    auto Bitmap::SetPoint(float x, float y, float z) -> void
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

    auto Bitmap::SetOpacity(float opacity) -> void
    {
    }

    auto Bitmap::GetWidth() const -> float
    {
        return 0.0f;
    }

    auto Bitmap::GetHeight() const -> float
    {
        return 0.0f;
    }

    auto Bitmap::GetPitch() const -> float
    {
        return 0.0f;
    }

    Bitmap::~Bitmap() = default;

    Bitmap::Bitmap(Bitmap&& other) = default;

    auto Bitmap::operator=(Bitmap&& other) -> Bitmap& = default;

} // namespace N503::Renderer2D
