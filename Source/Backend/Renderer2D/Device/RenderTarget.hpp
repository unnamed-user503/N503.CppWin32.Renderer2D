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
#include <memory>
#include <chrono>
#include <functional>

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device::Details
{
    class Timer
    {
    public:
        explicit Timer(const std::chrono::microseconds interval)
            : m_Interval(interval)
            , m_LastUpdateTime(std::chrono::high_resolution_clock::now())
        {
        }

        void Update(std::function<void()> callback)
        {
            auto now     = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - m_LastUpdateTime);

            if (elapsed >= m_Interval)
            {
                m_LastUpdateTime = now;
                callback();
            }
        }

    private:
        std::chrono::microseconds m_Interval;
        std::chrono::high_resolution_clock::time_point m_LastUpdateTime;
    };
}

namespace N503::Renderer2D::Device
{

    class RenderTarget
    {
    public:
        RenderTarget(Device::Context& context, HWND hwnd);

        auto Resize(UINT width, UINT height) -> void;

        auto Present() noexcept -> HRESULT;

        auto GetTargetWindow() const noexcept -> HWND
        {
            return m_TargetWindow;
        }

        auto GetTargetBitmap() const noexcept -> wil::com_ptr<ID2D1Bitmap1>
        {
            return m_TargetBitmap;
        }

        auto GetWidth() const noexcept
        {
            return m_Width;
        }

        auto GetHeight() const noexcept
        {
            return m_Height;
        }

    private:
        Device::Context& m_DeviceContext;

        wil::com_ptr<IDXGISwapChain1> m_SwapChain;

        wil::com_ptr<ID2D1Bitmap1> m_TargetBitmap;

        HWND m_TargetWindow{ nullptr };

        std::uint32_t m_Width{ 0 };

        std::uint32_t m_Height{ 0 };

        Details::Timer m_Timer{ std::chrono::microseconds(16000) };
    };

} // namespace N503::Renderer2D::Device
