#include "Pch.hpp"
#include "Window.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{

    auto Window::FindRenderTargetWindow() -> HWND
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

} // namespace N503::Renderer2D::Device
