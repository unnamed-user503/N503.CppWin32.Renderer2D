#include "Pch.hpp"
#include "Context.hpp"

// 1. Project Headers
#include "RenderTarget.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <dwrite.h>
#include <dxgi.h>
#include <windows.h>

// 6. C++ Standard Libraries
#include <memory>

namespace N503::Renderer2D::Device
{

    Context::Context()
    {
        // Direct2D Factory のリソースを作成
        D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        THROW_IF_FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, m_D2DFactory.put_void()));

        // DirectWrite Factory のリソースを作成
        wil::com_ptr<IUnknown> dwriteFactory;
        THROW_IF_FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), dwriteFactory.put()));
        m_DWriteFactory = dwriteFactory.query<IDWriteFactory>();

        // Direct3D11のリソースを作成する
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

        wil::com_ptr<ID3D11Device> baseDevice;
        wil::com_ptr<ID3D11DeviceContext> baseContext;

        THROW_IF_FAILED(::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            baseDevice.put(),
            nullptr,
            baseContext.put()
        ));

        m_D3DDevice  = baseDevice;
        m_D3DContext = baseContext;

        // Direct3D11のリソースからDirect2Dのリソースを作成する
        auto dxgiDevice = m_D3DDevice.query<IDXGIDevice>();

        THROW_IF_FAILED(m_D2DFactory->CreateDevice(dxgiDevice.get(), m_D2DDevice.put()));
        THROW_IF_FAILED(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_D2DContext.put()));
    }

    Context::~Context()
    {
        Destroy();
    }

    auto Context::BeginDraw() const noexcept -> bool
    {
        if (!m_RenderTarget || !m_D2DContext)
        {
            return false;
        }

        m_D2DContext->SetTarget(m_RenderTarget->GetTargetBitmap());
        m_D2DContext->BeginDraw();
        m_D2DContext->Clear(D2D1::ColorF(0.1f, 0.15f, 0.3f, 1.0f));
        return true;
    }

    auto Context::EndDraw() const noexcept -> HRESULT
    {
        if (!m_D2DContext)
        {
            return E_FAIL;
        }

        return m_D2DContext->EndDraw();
    }

    auto Context::Present() const noexcept -> HRESULT
    {
        if (!m_RenderTarget || !m_D2DContext)
        {
            return E_FAIL;
        }

        return m_RenderTarget->Present();
    }

    auto Context::Destroy() noexcept -> void
    {
        if (m_RenderTarget)
        {
            m_RenderTarget.reset();
        }

        if (m_D2DContext)
        {
            m_D2DContext->SetTarget(nullptr);
            m_D2DContext.reset();
        }

        if (m_D2DDevice)
        {
            m_D2DDevice.reset();
        }

        if (m_D3DContext)
        {
            m_D3DContext.reset();
        }

        if (m_D3DDevice)
        {
            m_D3DDevice.reset();
        }

        if (m_DWriteFactory)
        {
            m_DWriteFactory.reset();
        }

        if (m_D2DFactory)
        {
            m_D2DFactory.reset();
        }
    }

    auto Context::SetRenderTarget(HWND hwnd) -> void
    {
        if (!hwnd || m_RenderTarget)
        {
            return;
        }

        if (m_D2DContext)
        {
            m_D2DContext->SetTarget(nullptr);
        }

        m_RenderTarget = std::make_unique<RenderTarget>(hwnd, *this);
    }

} // namespace N503::Renderer2D::Device
