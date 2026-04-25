#include "Pch.hpp"
#include "CreateBitmap.hpp"

// 1. Project Headers
#include "../../Codec/WicImageDecoder.hpp"
#include "../../Engine.hpp"
#include "../../Message/Context.hpp"
#include "../../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <cstddef>
#include <span>

namespace N503::Renderer2D::Message::Packets
{

    auto CreateBitmap::operator()(Context& context) const -> void
    {
        if (Path.empty())
        {
            return;
        }

        Codec::WicImageDecoder decoder(Path);

        Pixels::Buffer pixels;
        pixels.Width  = decoder.GetWidth();
        pixels.Height = decoder.GetHeight();
        pixels.Pitch  = decoder.GetPitch();
        pixels.Bytes  = nullptr;
        pixels.Size   = static_cast<std::size_t>(pixels.Height) * pixels.Pitch;

        auto handle = Engine::GetInstance().GetResourceContainer().Add(Path, pixels); // 書き込み可能なpixels.Bytesを設定

        if (!handle)
        {
            return;
        }

        decoder.Decode([&pixels](auto size) { return std::span<std::byte>(pixels.Bytes, std::min(size, pixels.Size)); });

        *Result = handle;
        *Pixels = pixels;
    }

} // namespace N503::Renderer2D::Message::Packets
