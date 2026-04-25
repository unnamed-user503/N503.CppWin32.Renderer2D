#include "Pch.hpp"
#include "Context.hpp"

// 1. Project Headers
#include "RenderTarget.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi.h>
#include <windows.h>

// 6. C++ Standard Libraries

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

        wil::com_ptr<ID3D11Device> d3d11Device;
        wil::com_ptr<ID3D11DeviceContext> d3d11Context;

        THROW_IF_FAILED(::D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            d3d11Device.put(),
            nullptr,
            d3d11Context.put()
        ));

        m_D3DDevice  = d3d11Device;
        m_D3DContext = d3d11Context;

        // Direct3D11のリソースからDirect2Dのリソースを作成する
        auto dxgiDevice = m_D3DDevice.query<IDXGIDevice>();

        THROW_IF_FAILED(m_D2DFactory->CreateDevice(dxgiDevice.get(), m_D2DDevice.put()));
        THROW_IF_FAILED(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, m_D2DContext.put()));
    }

    auto Context::GetRenderTargetWindow() const noexcept -> HWND
    {
        wil::unique_event_nothrow createWindowCompletion;

        if (!createWindowCompletion.try_open(L"Local\\N503.CppWin32.Event.Core.CreateWindowCompletion", SYNCHRONIZE | EVENT_MODIFY_STATE))
        {
            return {};
        }

        if (::WaitForSingleObject(createWindowCompletion.get(), 0) != WAIT_OBJECT_0)
        {
            return {};
        }

        struct Context
        {
            HWND Result{};
            DWORD TargetPID{};
        };

        Context context{ nullptr, ::GetCurrentProcessId() };

        auto callback = [](HWND handle, LPARAM lParam) -> BOOL
        {
            auto pContext = reinterpret_cast<Context*>(lParam);

            DWORD pid{};
            ::GetWindowThreadProcessId(handle, &pid);

            if (pid == pContext->TargetPID)
            {
                wchar_t className[256]{};
                ::GetClassNameW(handle, className, 256);

                if (std::wcscmp(className, L"N503.CppWin32.Core.Window") == 0)
                {
                    pContext->Result = handle;
                    return FALSE;
                }
            }

            return TRUE;
        };

        ::EnumWindows(callback, reinterpret_cast<LPARAM>(&context));

        if (!context.Result)
        {
            return {};
        }
        else
        {
            // 成功したので旗を下ろす
            createWindowCompletion.ResetEvent();
        }

        return context.Result;
    }

    auto Context::BeginDraw() const noexcept -> bool
    {
        if (!m_D2DContext)
        {
            return false;
        }

        m_D2DContext->BeginDraw();
        m_D2DContext->Clear(D2D1::ColorF(0.1f, 0.2, 0.4f));
        return true;
    }

    auto Context::EndDraw() const noexcept -> HRESULT
    {
        // D2D の描画命令を確定させる
        HRESULT hr = m_D2DContext->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            // デバイスロスト発生。外部に通知してコンテキストを再構築させる
        }

        return hr;
    }

    auto Context::SetTarget(const RenderTarget& renderTarget) const noexcept -> void
    {
        m_D2DContext->SetTarget(renderTarget.GetTargetBitmap().get());
    }

} // namespace N503::Renderer2D::Device
