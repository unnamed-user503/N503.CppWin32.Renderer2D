#include "Pch.hpp"
#include "WicImageDecoder.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "Decoder.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Codec
{
    // コンストラクタでデコーダーの初期化とメタ情報の確定を行う
    WicImageDecoder::WicImageDecoder(std::string_view path)
    {
        auto factory = wil::CoCreateInstance<IWICImagingFactory>(CLSID_WICImagingFactory);

        // パスの変換 (UTF-8 -> UTF-16)
        std::wstring wpath(path.begin(), path.end());

        // デコーダーの生成 (Audio の SourceReader 生成に相当)
        THROW_IF_FAILED(factory->CreateDecoderFromFilename(wpath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &m_decoder));
    }

    auto WicImageDecoder::Decode(Allocator allocator) -> Pixels::Buffer
    {
        // フレームの取得
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        THROW_IF_FAILED(m_decoder->GetFrame(0, &frame));

        // フォーマットコンバーターの設定 (32bppPBGRA 固定)
        auto factory = wil::CoCreateInstance<IWICImagingFactory>(CLSID_WICImagingFactory);
        wil::com_ptr<IWICFormatConverter> converter;
        THROW_IF_FAILED(factory->CreateFormatConverter(&converter));

        THROW_IF_FAILED(converter->Initialize(frame.get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom));

        // 必要なメモリサイズの計算 (Audio の totalBytesNeeded に相当)
        const std::size_t totalBytesNeeded = GetRequiredBufferSize();

        // アロケータを介したメモリ確保 (Audio と全く同じフロー)
        auto target = allocator(totalBytesNeeded);

        if (target.empty() || target.size() < totalBytesNeeded)
        {
            return {};
        }

        // ピクセルのコピー (Audio の ReadBytes に相当)
        const std::uint32_t pitch = GetPitch();
        THROW_IF_FAILED(converter->CopyPixels(nullptr, pitch, static_cast<UINT>(target.size()), reinterpret_cast<BYTE*>(target.data())));

#ifdef _DEBUG
        Renderer2D::Engine::Instance().GetDiagnosticsSink().AddEntry(Diagnostics::Entry{
            .Severity = Diagnostics::Severity::Verbose,
            .Expected = std::format("Decoded image: {} ({}x{}, {} bytes)", std::string(GetWidth(), 'x'), GetWidth(), GetHeight(), totalBytesNeeded),
            .Position = 0 });
#endif

        // デコード結果を Buffer として返却
        return { .Bytes = target.data(), .Size = totalBytesNeeded, .Width = GetWidth(), .Height = GetHeight(), .Pitch = pitch };
    }

    auto WicImageDecoder::GetWidth() const -> std::uint32_t
    {
        std::uint32_t w, h;
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        if (SUCCEEDED(m_decoder->GetFrame(0, &frame)))
        {
            frame->GetSize(&w, &h);
            return w;
        }
        return 0;
    }

    auto WicImageDecoder::GetHeight() const -> std::uint32_t
    {
        std::uint32_t w, h;
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        if (SUCCEEDED(m_decoder->GetFrame(0, &frame)))
        {
            frame->GetSize(&w, &h);
            return h;
        }
        return 0;
    }

    auto WicImageDecoder::GetPitch() const -> std::uint32_t
    {
        // 32bppPBGRA (4 bytes per pixel)
        return GetWidth() * 4;
    }

} // namespace N503::Renderer2D::Codec
