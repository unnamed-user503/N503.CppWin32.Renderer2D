#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Details/Api.h>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>

namespace N503::Renderer2D
{

    namespace Geometry
    {
        using Vector2F    = N503Vector2F;
        using Vector3F    = N503Vector3F;
        using RectF       = N503RectF;
        using RectU       = N503RectU;
        using Transform2D = N503Transform2D;
        using Transform3D = N503Transform3D;
    } // namespace Geometry

    using ColorF = N503Color_t;

    enum class PixelFormat
    {
        Unknown = 0,

        B8G8R8A8_UNORM,
    };

    enum class RenderGroup : std::uint8_t
    {
        Background = 0,
        World,
        Normal,
        Text,
        Effect,
        UI,
        Threshold,
    };

    namespace Handle
    {

        enum class Generation : std::uint64_t
        {
            Default = 0,
        };

        enum class ResourceID : std::uint64_t
        {
            Invalid = std::uint64_t(-1),
        };

        enum class ResourceType : std::uint8_t
        {
            None = 0,
            Bitmap,
            Movie,
            Text,
        };

        enum class CommandID : std::uint64_t
        {
            Invalid = std::uint64_t(-1),
        };

        enum class CommandType : std::uint8_t
        {
            None = 0,
            Draw,
            System,
        };

    } // namespace Handle

    struct ResourceHandle
    {
        Handle::ResourceID ID{ Handle::ResourceID::Invalid };

        Handle::ResourceType Type;

        Handle::Generation Generation;

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ID != Handle::ResourceID::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ID == Handle::ResourceID::Invalid;
        }

        [[nodiscard]]
        bool operator==(const ResourceHandle&) const = default;

        [[nodiscard]]
        bool operator==(const Handle::ResourceID& other) const
        {
            return ID == other;
        }

        [[nodiscard]]
        bool operator!=(const Handle::ResourceID& other) const
        {
            return ID != other;
        }
    };

    struct CommandHandle
    {
        Handle::CommandID ID{ Handle::CommandID::Invalid };

        Handle::CommandType Type;

        Handle::Generation Generation;

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ID != Handle::CommandID::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ID == Handle::CommandID::Invalid;
        }

        [[nodiscard]]
        bool operator==(const CommandHandle&) const = default;

        [[nodiscard]]
        bool operator==(const Handle::CommandID& other) const
        {
            return ID == other;
        }

        [[nodiscard]]
        bool operator!=(const Handle::CommandID& other) const
        {
            return ID != other;
        }
    };

} // namespace N503::Renderer2D
