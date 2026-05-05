#include "Pch.hpp"
#include "Surface.hpp"

// 1. Project Headers
#include "Device.hpp"
#include "Session.hpp"

// 3. WIL
#include <wil/result_macros.h>

namespace N503::Renderer2D::Canvas
{
    Surface::Surface(Device& device, HWND hwnd) : m_Device(device), m_TargetWindow(hwnd)
    {
        // 1. DXGI Factory の取得[cite: 9]
        // Device::D3dDevice() は ID3D11Device& を返すため、アドレスを取得してクエリ[cite: 8]
        auto dxgiDevice = wil::com_query<IDXGIDevice>(&m_Device.GetD3D11Device());

        wil::com_ptr<IDXGIAdapter> dxgiAdapter;
        THROW_IF_FAILED(dxgiDevice->GetAdapter(dxgiAdapter.put()));

        wil::com_ptr<IDXGIFactory2> dxgiFactory;
        THROW_IF_FAILED(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.put())));

        // 2. スワップチェーンの設定[cite: 9]
        DXGI_SWAP_CHAIN_DESC1 description{};
        description.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.SampleDesc.Count = 1;
        description.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        description.BufferCount      = 2;
        description.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

        // 3. スワップチェーンの生成[cite: 9]
        THROW_IF_FAILED(dxgiFactory->CreateSwapChainForHwnd(&m_Device.GetD3D11Device(), m_TargetWindow, &description, nullptr, nullptr, m_SwapChain.put()));

        // 4. 初期サイズの取得と適用
        RECT rc;
        ::GetClientRect(m_TargetWindow, &rc);
        Resize(rc.right - rc.left, rc.bottom - rc.top);
    }

    auto Surface::Begin(const D2D1_COLOR_F& clearColor) -> Session
    {
        auto& context = m_Device.GetD2D1DeviceContext(); // 参照を取得[cite: 8]

        context.SetTarget(m_TargetBitmap.get());
        context.BeginDraw();
        context.SetUnitMode(D2D1_UNIT_MODE_PIXELS);
        context.Clear(clearColor);

        return Session{ &m_Device };
    }

    auto Surface::Resize(UINT32 width, UINT32 height) -> void
    {
        if (width == 0 || height == 0)
        {
            return;
        }

        // 既存のターゲットを解放しないと ResizeBuffers は失敗する[cite: 9]
        m_TargetBitmap.reset();

        THROW_IF_FAILED(m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0));

        // バックバッファから新しい D2D ターゲットビットマップを作成[cite: 9]
        wil::com_ptr<IDXGISurface> backBuffer;
        THROW_IF_FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.put())));

        const auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

        THROW_IF_FAILED(m_Device.GetD2D1DeviceContext().CreateBitmapFromDxgiSurface(backBuffer.get(), &props, m_TargetBitmap.put()));

        m_Width  = width;
        m_Height = height;
    }

    auto Surface::Present(UINT syncInterval) noexcept -> HRESULT
    {
        return m_SwapChain->Present(syncInterval, 0); //[cite: 9, 10]
    }

} // namespace N503::Renderer2D::Canvas
