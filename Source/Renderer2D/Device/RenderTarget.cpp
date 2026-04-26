#include "Pch.hpp"
#include "RenderTarget.hpp"

// 1. Project Headers
#include "Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <dcommon.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgiformat.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{

    RenderTarget::RenderTarget(Device::Context& context, HWND hwnd) : m_TargetWindow(hwnd)
    {
        auto d3dDevice  = context.GetD3DDevice();
        auto dxgiDevice = d3dDevice.query<IDXGIDevice>();

        wil::com_ptr<IDXGIAdapter> dxgiAdapter;
        THROW_IF_FAILED(dxgiDevice->GetAdapter(dxgiAdapter.put()));

        wil::com_ptr<IDXGIFactory2> dxgiFactory;
        THROW_IF_FAILED(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.put())));

        DXGI_SWAP_CHAIN_DESC1 description{};
        description.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.SampleDesc.Count = 1;
        description.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.BufferCount      = 2;
        description.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        THROW_IF_FAILED(dxgiFactory->CreateSwapChainForHwnd(d3dDevice.get(), m_TargetWindow, &description, nullptr, nullptr, m_SwapChain.put()));

        wil::com_ptr<IDXGISurface> backBuffer;
        THROW_IF_FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.put())));

        D2D1_BITMAP_PROPERTIES1
        bitmapProperties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

        THROW_IF_FAILED(context.GetD2DContext()->CreateBitmapFromDxgiSurface(backBuffer.get(), &bitmapProperties, m_TargetBitmap.put()));
    }

    auto RenderTarget::Resize(const Device::Context& context, UINT width, UINT height) -> void
    {
        if (!m_SwapChain)
        {
            return;
        }

        // 古いターゲットを破棄
        m_TargetBitmap.reset();

        // SwapChain のサイズ変更
        THROW_IF_FAILED(m_SwapChain->ResizeBuffers(
            0, // バッファ数は変更しない
            width,
            height,
            DXGI_FORMAT_UNKNOWN,
            0
        ));

        // 新しい BackBuffer を取得
        wil::com_ptr<IDXGISurface> backBuffer;
        THROW_IF_FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.put())));

        // D2D Bitmap 再作成
        D2D1_BITMAP_PROPERTIES1
        bitmapProps = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        THROW_IF_FAILED(context.GetD2DContext()->CreateBitmapFromDxgiSurface(backBuffer.get(), &bitmapProps, m_TargetBitmap.put()));
    }

    auto RenderTarget::Present() const noexcept -> HRESULT
    {
        return m_SwapChain->Present(1, 0);
    }

} // namespace N503::Renderer2D::Device
