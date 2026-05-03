#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <wincodec.h>

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>

namespace N503::Renderer2D::Codec
{

    class WicImageDecoder final
    {
    public:
        WicImageDecoder(const std::string_view path);

        auto Decode(std::function<std::span<std::byte>(std::size_t)> allocate) -> Pixels::Buffer;

        auto GetWidth() const -> std::uint32_t;

        auto GetHeight() const -> std::uint32_t;

        auto GetPitch() const -> std::uint32_t;

        auto GetPath() const -> std::string_view;

    private:
        wil::com_ptr<IWICBitmapDecoder> m_Decoder;

        std::string m_Path;
    };

} // namespace N503::Renderer2D::Codec
