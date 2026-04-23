#include "Pch.hpp"
#include "Bitmap_Entity.hpp"
#include <N503/Renderer2D/Bitmap.hpp>

// 1. Project Headers
#include "Command/Packets/CreateResourceCommand.hpp"
#include "Command/Packets/DestroyResourceCommand.hpp"
#include "Command/Queue.hpp"
#include "Engine.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <memory>
#include <utility>

namespace N503::Renderer2D
{

    /// @brief
    Bitmap::Bitmap(const std::string_view path)
    {
        m_Entity = std::make_unique<Bitmap::Entity>();

        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::CreateResourceCommand{ .Path = path };

        Renderer2D::Engine::Instance().Start();
        Renderer2D::Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    Bitmap::~Bitmap() = default;

    Bitmap::Bitmap(Bitmap&&) noexcept = default;

    auto Bitmap::operator=(Bitmap&&) noexcept -> Bitmap& = default;

} // namespace N503::Renderer2D
