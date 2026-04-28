#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <unordered_map>

namespace N503::Renderer2D::Pixels
{
    struct Buffer;
}

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device::Resource
{

    class Cache
    {
    public:
        Cache(Device::Context& context);

        auto Get(ResourceHandle handle) -> wil::com_ptr<ID2D1Bitmap1>;

        auto GetOrCreateBitmap(ResourceHandle handle, const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>;

        auto GetOrCreateBrush(const Renderer2D::ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>;

        auto GetOrCreateTextFormat(std::string_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;

        auto GetOrCreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;

        auto GetOrCreateTextLayout(const std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;

        auto GetOrCreateTextLayout(const std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;

    private:
        Device::Context& m_DeviceContext;

        std::unordered_map<Handle::ResourceID, wil::com_ptr<ID2D1Bitmap1>> m_Bitmaps;

        std::unordered_map<std::uint32_t, wil::com_ptr<ID2D1SolidColorBrush>> m_Brushes;

        std::unordered_map<std::wstring, wil::com_ptr<IDWriteTextFormat>> m_TextFormats;

        std::unordered_map<std::wstring, wil::com_ptr<IDWriteTextLayout>> m_TextLayouts;
    };

} // namespace N503::Renderer2D::Device::Resource
