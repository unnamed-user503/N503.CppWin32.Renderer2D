#pragma once

// 1. Project Headers
#include "Resource/Cache.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi1_2.h>

// 6. C#include <memory>
#include <cstdint>
#include <string_view>

namespace N503::Renderer2D::Pixels
{
    struct Buffer;
}

namespace N503::Renderer2D::Device
{
    class RenderTarget;
}

namespace N503::Renderer2D::Device
{

    class Context
    {
    public:
        Context();

    public:
        auto CreateBitmap(const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>;

        auto CreateSolidColorBrush(const Renderer2D::ColorF color) -> wil::com_ptr<ID2D1SolidColorBrush>;

        auto CreateTextFormat(const std::string_view fontName, const float fontSize) -> wil::com_ptr<IDWriteTextFormat>;

        auto CreateTextFormat(const std::wstring_view fontName, const float fontSize) -> wil::com_ptr<IDWriteTextFormat>;

        auto CreateTextLayout(const std::string_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;

        auto CreateTextLayout(const std::wstring_view text, wil::com_ptr<IDWriteTextFormat> textFormat) -> wil::com_ptr<IDWriteTextLayout>;

        auto DrawBitmap(wil::com_ptr<ID2D1Bitmap1> bitmap, const D2D1_RECT_F destination) -> void;

        auto DrawTextLayout(const Renderer2D::PointF origin, wil::com_ptr<IDWriteTextLayout> textLayout, wil::com_ptr<ID2D1SolidColorBrush> brush) -> void;

        auto SetTransform(const D2D1_MATRIX_3X2_F& transform) -> void;

    public:
        auto BeginDraw(const Renderer2D::ColorF clearColor) -> bool;

        auto EndDraw() -> HRESULT;

        auto Reset() -> void;

        auto SetRenderTarget(RenderTarget* renderTarget) -> void;

        auto GetD3DDevice() const -> wil::com_ptr<ID3D11Device>
        {
            return m_D3DDevice;
        }

        auto GetD2DContext() const -> wil::com_ptr<ID2D1DeviceContext>
        {
            return m_D2DContext;
        }

        auto GetD2DContext3() const -> wil::com_ptr<ID2D1DeviceContext3>
        {
            return m_D2DContext3;
        }

        auto GetSpriteBatch() const -> wil::com_ptr<ID2D1SpriteBatch>
        {
            return m_SpriteBatch;
        }

        auto GetResourceCache() -> Resource::Cache&
        {
            return m_ResourceCache;
        }

    private:
        wil::com_ptr<ID2D1Factory1> m_D2DFactory;

        wil::com_ptr<IDWriteFactory> m_DWriteFactory;

        wil::com_ptr<ID3D11Device> m_D3DDevice;

        wil::com_ptr<ID3D11DeviceContext> m_D3DContext;

        wil::com_ptr<ID2D1Device> m_D2DDevice;

        wil::com_ptr<ID2D1DeviceContext> m_D2DContext;

        wil::com_ptr<ID2D1DeviceContext3> m_D2DContext3;

        wil::com_ptr<ID2D1SpriteBatch> m_SpriteBatch;

        Resource::Cache m_ResourceCache{ *this };
    };

} // namespace N503::Renderer2D::Device
