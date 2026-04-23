#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <functional>
#include <span>

namespace N503::Renderer2D::Codec
{

    class Decoder
    {
    public:
        using Allocator = std::function<std::span<std::byte>(std::size_t)>;

    public:
        virtual ~Decoder() = default;

        virtual auto Decode(Allocator allocator) -> Pixels::Buffer = 0;

        virtual auto GetWidth() const -> std::uint32_t = 0;

        virtual auto GetHeight() const -> std::uint32_t = 0;

        virtual auto GetPitch() const -> std::uint32_t = 0;

        virtual auto GetRequiredBufferSize() const -> std::size_t
        {
            return static_cast<std::size_t>(GetPitch()) * GetHeight();
        }
    };

} // namespace N503::Renderer2D::Codec
