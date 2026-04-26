#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <dxgi1_2.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device
{

    class RenderTarget
    {
    public:
        RenderTarget(Device::Context& context, HWND hwnd);

        auto Resize(const Device::Context& context, UINT width, UINT height) -> void;

        auto Present() const noexcept -> HRESULT;

        auto GetTargetWindow() const noexcept -> HWND
        {
            return m_TargetWindow;
        }

        auto GetTargetBitmap() const noexcept -> wil::com_ptr<ID2D1Bitmap1>
        {
            return m_TargetBitmap;
        }

    private:
        wil::com_ptr<IDXGISwapChain1> m_SwapChain;

        wil::com_ptr<ID2D1Bitmap1> m_TargetBitmap;

        HWND m_TargetWindow;
    };

} // namespace N503::Renderer2D::Device
