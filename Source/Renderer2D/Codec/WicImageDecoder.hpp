#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Decoder.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <wincodec.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Codec
{

    class WicImageDecoder : public Decoder
    {
    public:
        WicImageDecoder(std::string_view path);

        virtual ~WicImageDecoder() = default;

        virtual auto Decode(Allocator allocator) -> Pixels::Buffer override;

        virtual auto GetWidth() const -> std::uint32_t override;

        virtual auto GetHeight() const -> std::uint32_t override;

        virtual auto GetPitch() const -> std::uint32_t override;

    private:
        wil::com_ptr<IWICBitmapDecoder> m_decoder;
    };

} // namespace N503::Renderer2D::Codec
