#include "Pch.hpp"
#include "Bitmap_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packets/AddCommandList.hpp"
#include "Message/Packets/CreateBitmap.hpp"
#include "Message/Packets/UpdateBitmapOpacity.hpp"
#include "Message/Packets/UpdateBitmapPoint.hpp"

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

        auto packet = Message::Packets::CreateBitmap{
            .Result = &m_Entity->ResourceHandle,
            .Pixels = &m_Entity->Pixels,
            .Path   = path,
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

        auto packet2 = Message::Packets::AddCommandList{
            .Result         = &m_Entity->CommandHandle,
            .ResourceHandle = m_Entity->ResourceHandle,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet2));
    }

    auto Bitmap::SetPoint(float x, float y) -> void
    {
        if (m_Entity == nullptr)
        {
            return;
        }

        auto packet = Message::Packets::UpdateBitmapPoint{
            .CommandHandle = m_Entity->CommandHandle,
            .Point         = PointF{ x, y },
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Bitmap::SetOpacity(float opacity) -> void
    {
        if (m_Entity == nullptr)
        {
            return;
        }

        auto packet = Message::Packets::UpdateBitmapOpacity{
            .CommandHandle = m_Entity->CommandHandle,
            .Opacity       = opacity,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Bitmap::GetWidth() const -> float
    {
        if (m_Entity == nullptr)
        {
            return 0.0f;
        }

        return m_Entity->Pixels.Width;
    }

    auto Bitmap::GetHeight() const -> float
    {
        if (m_Entity == nullptr)
        {
            return 0.0f;
        }

        return m_Entity->Pixels.Height;
    }

    auto Bitmap::GetPitch() const -> float
    {
        if (m_Entity == nullptr)
        {
            return 0.0f;
        }

        return m_Entity->Pixels.Pitch;
    }

    Bitmap::~Bitmap() = default;

    Bitmap::Bitmap(Bitmap&& other) = default;

    auto Bitmap::operator=(Bitmap&& other) -> Bitmap& = default;

} // namespace N503::Renderer2D
