#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <type_traits>

namespace N503::Renderer2D
{

    enum class Type : std::uint16_t
    {
        None,
        Bitmap,
        Movie,
    };

    namespace Handle
    {
        enum class Ticket : std::uint64_t
        {
            InvalidValue = std::uint64_t(-1)
        };

        enum class Tag : std::uint64_t
        {
            InvalidValue = std::uint64_t(-1)
        };

        enum class ResourceID : std::uint64_t
        {
            InvalidValue = 0
        };

        enum class Generation : std::uint32_t
        {
            InitialValue = 0
        };
    } // namespace Handle

    struct ProcessHandle final
    {
        Handle::Tag Tag{ Handle::Tag::InvalidValue };

        Handle::Ticket Ticket{ Handle::Ticket::InvalidValue };

        Handle::Generation Generation{ Handle::Generation::InitialValue };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return Ticket != Handle::Ticket::InvalidValue;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return Ticket == Handle::Ticket::InvalidValue;
        }
    };

    struct AssetHandle final
    {
        Handle::ResourceID ResourceID{ Handle::ResourceID::InvalidValue };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ResourceID != Handle::ResourceID::InvalidValue;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ResourceID == Handle::ResourceID::InvalidValue;
        }

        // 比較演算子（map のキーには必須）
        [[nodiscard]]
        bool operator==(const AssetHandle&) const = default;
    };

    inline auto operator++(Renderer2D::Handle::Tag& tag) noexcept -> Renderer2D::Handle::Tag&
    {
        using T = std::underlying_type_t<Renderer2D::Handle::Tag>;
        tag     = static_cast<Renderer2D::Handle::Tag>(static_cast<T>(tag) + 1);
        return tag;
    }

    inline auto operator++(Renderer2D::Handle::Generation& generation) noexcept -> Renderer2D::Handle::Generation&
    {
        using T    = std::underlying_type_t<Renderer2D::Handle::Generation>;
        generation = static_cast<Renderer2D::Handle::Generation>(static_cast<T>(generation) + 1);
        return generation;
    }

} // namespace N503::Renderer2D

// ハッシュ関数の定義
namespace std
{

    template <> struct hash<N503::Renderer2D::AssetHandle>
    {
        size_t operator()(const N503::Renderer2D::AssetHandle& handle) const noexcept
        {
            // ResourceID が enum class なら underlying_type でキャスト
            using T = std::underlying_type_t<N503::Renderer2D::Handle::ResourceID>;
            return std::hash<T>{}(static_cast<T>(handle.ResourceID));
        }
    };

} // namespace std
