#include "Pch.hpp"
#include "WicImageDecoder.hpp"

// 1. Project Headers
#include "../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
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

    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    WicImageDecoder::WicImageDecoder(const std::string_view path) : m_Path(path)
    {
        auto factory = wil::CoCreateInstance<IWICImagingFactory>(CLSID_WICImagingFactory);

        // デコーダーを作成して、画像ファイルを読み込む
        THROW_IF_FAILED(factory->CreateDecoderFromFilename(TranscodeUtf8ToWide(path).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &m_Decoder)
        );
    }

    auto WicImageDecoder::Decode(std::function<std::span<std::byte>(std::size_t)> allocate) -> Pixels::Buffer
    {
        // 画像の最初のフレームを取得する
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        THROW_IF_FAILED(m_Decoder->GetFrame(0, &frame));

        // フォーマットコンバーターを作成して、フレームを32bppPBGRA形式に変換する
        auto factory = wil::CoCreateInstance<IWICImagingFactory>(CLSID_WICImagingFactory);
        wil::com_ptr<IWICFormatConverter> converter;
        THROW_IF_FAILED(factory->CreateFormatConverter(&converter));

        // 32bppPBGRA形式に変換する
        THROW_IF_FAILED(converter->Initialize(frame.get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom));

        // 画像の幅、高さ、ピッチを取得して、必要なバイト数を計算する
        const std::uint32_t width          = GetWidth();
        const std::uint32_t height         = GetHeight();
        const std::uint32_t pitch          = GetPitch();
        const std::size_t totalBytesNeeded = static_cast<std::size_t>(pitch) * height;

        // バッファを割り当てる
        auto target = allocate(totalBytesNeeded);

        if (target.empty() || target.size() < totalBytesNeeded)
        {
            return {};
        }

        // 変換されたピクセルデータをバッファにコピーする
        THROW_IF_FAILED(converter->CopyPixels(nullptr, pitch, static_cast<UINT>(target.size()), reinterpret_cast<BYTE*>(target.data())));

        // バッファを返す
        return {
            .Bytes  = target.data(),
            .Size   = totalBytesNeeded,
            .Width  = GetWidth(),
            .Height = GetHeight(),
            .Pitch  = pitch,
        };
    }

    auto WicImageDecoder::GetWidth() const -> std::uint32_t
    {
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        m_Decoder->GetFrame(0, &frame);

        UINT width  = 0;
        UINT height = 0;

        frame->GetSize(&width, &height);
        return width;
    }

    auto WicImageDecoder::GetHeight() const -> std::uint32_t
    {
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        m_Decoder->GetFrame(0, &frame);

        UINT width  = 0;
        UINT height = 0;

        frame->GetSize(&width, &height);
        return height;
    }

    auto WicImageDecoder::GetPitch() const -> std::uint32_t
    {
        return GetWidth() * 4;
    }

    auto WicImageDecoder::GetPath() const -> std::string_view
    {
        return m_Path;
    }

} // namespace N503::Renderer2D::Codec
