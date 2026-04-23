#pragma once

#include <windows.h>

#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dwrite.h>
#include <dxgi1_2.h>

#include "Context.hpp"
#include <wil/com.h>

namespace N503::Renderer2D::Device
{

    class RenderTarget final
    {
    public:
        explicit RenderTarget(HWND handle, const Context& context);
        ~RenderTarget() = default;

        RenderTarget(const RenderTarget&)                    = delete;
        auto operator=(const RenderTarget&) -> RenderTarget& = delete;

    public:
        auto Present() const noexcept -> HRESULT;

        auto GetTargetWindow() const noexcept -> HWND
        {
            return m_TargetWindow;
        }

        auto GetTargetBitmap() const noexcept -> ID2D1Bitmap1*
        {
            return m_TargetBitmap.get();
        }

    private:
        wil::com_ptr<IDXGISwapChain1> m_SwapChain;
        wil::com_ptr<ID2D1Bitmap1> m_TargetBitmap;

        HWND m_TargetWindow{ nullptr };
    };

} // namespace N503::Renderer2D::Device
