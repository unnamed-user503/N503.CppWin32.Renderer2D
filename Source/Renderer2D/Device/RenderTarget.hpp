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
#include <dxgi1_2.h>
#include <windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{

    class Context;

    class RenderTarget
    {
    public:
        RenderTarget(const Context* context, HWND hwnd);

        RenderTarget(const RenderTarget&) = delete;

        auto operator=(const RenderTarget&) -> RenderTarget& = delete;

    public:
        auto Present() const noexcept -> HRESULT;

        auto GetTargetBitmap() const noexcept -> const wil::com_ptr<ID2D1Bitmap1>&
        {
            return m_TargetBitmap;
        }

        auto GetTargetWindow() const -> HWND
        {
            return m_TargetWindow;
        }

    private:
        wil::com_ptr<IDXGISwapChain1> m_SwapChain;

        wil::com_ptr<ID2D1Bitmap1> m_TargetBitmap;

        HWND m_TargetWindow{ nullptr };
    };

} // namespace N503::Renderer2D::Device
