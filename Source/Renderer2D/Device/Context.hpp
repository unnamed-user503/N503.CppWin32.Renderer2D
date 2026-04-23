#pragma once

#include <windows.h>

#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>

#include <memory>
#include <wil/com.h>
#include <wil/resource.h>

namespace N503::Renderer2D::Device
{

    class RenderTarget;

    class Context final
    {
    public:
        Context();
        ~Context();

        Context(const Context&)                    = delete;
        auto operator=(const Context&) -> Context& = delete;

    public:
        // RenderTargetがSwapChainを作る際に必要
        auto GetD3DDevice() const noexcept -> wil::com_ptr<ID3D11Device>
        {
            return m_D3DDevice;
        }

        // 実際の描画命令を発行する際に必要
        auto GetD2DContext() const noexcept -> wil::com_ptr<ID2D1DeviceContext>
        {
            return m_D2DContext;
        }

        auto BeginDraw() const noexcept -> bool;

        auto EndDraw() const noexcept -> HRESULT;

        auto Present() const noexcept -> void;

        auto Destroy() noexcept -> void;

        auto SetRenderTarget(HWND hwnd) -> void;

    private:
        wil::com_ptr<ID2D1Factory1> m_D2DFactory;
        wil::com_ptr<IDWriteFactory> m_DWriteFactory;

        wil::com_ptr<ID3D11Device> m_D3DDevice;
        wil::com_ptr<ID3D11DeviceContext> m_D3DContext;

        wil::com_ptr<ID2D1Device> m_D2DDevice;
        wil::com_ptr<ID2D1DeviceContext> m_D2DContext;

        std::unique_ptr<RenderTarget> m_RenderTarget;
    };

} // namespace N503::Renderer2D::Device
