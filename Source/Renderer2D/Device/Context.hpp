#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class RenderTarget;
}

namespace N503::Renderer2D::Device
{

    class Context final
    {
    public:
        Context();

    public:
        auto GetRenderTargetWindow() const noexcept -> HWND;

        auto BeginDraw() const noexcept -> bool;

        auto EndDraw() const noexcept -> HRESULT;

        auto SetTarget(const RenderTarget& renderTarget) const noexcept -> void;

    public:
        auto GetD3DDevice() const noexcept -> wil::com_ptr<ID3D11Device>
        {
            return m_D3DDevice;
        }

        auto GetD2DContext() const noexcept -> wil::com_ptr<ID2D1DeviceContext>
        {
            return m_D2DContext;
        }

    private:
        wil::com_ptr<ID2D1Factory1> m_D2DFactory;
        wil::com_ptr<IDWriteFactory> m_DWriteFactory;

        wil::com_ptr<ID3D11Device> m_D3DDevice;
        wil::com_ptr<ID3D11DeviceContext> m_D3DContext;

        wil::com_ptr<ID2D1Device> m_D2DDevice;
        wil::com_ptr<ID2D1DeviceContext> m_D2DContext;
    };

} // namespace N503::Renderer2D::Device
