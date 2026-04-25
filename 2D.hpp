#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Abi/Api.hpp>

// 3. WIL (Windows Implementation Library)
#include <N503/Renderer2D/Types.hpp>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string_view>

namespace N503::Renderer2D
{

    class N503_API Bitmap final
    {
    public:
        explicit Bitmap(const std::string_view path);

        ~Bitmap();

        Bitmap(const Bitmap&) = delete;

        auto operator=(const Bitmap&) -> Bitmap& = delete;

        Bitmap(Bitmap&&);

        auto operator=(Bitmap&&) -> Bitmap&;

    public:
        auto SetPoint(float x, float y) -> void;

        auto SetOpacity(float opacity) -> void;

        auto GetWidth() const -> float;

        auto GetHeight() const -> float;

        auto GetPitch() const -> float;

    public:
        struct Entity;

#ifdef N503_DLL_EXPORTS
        /// @brief 冁E��の実裁E��体！Entity�E�への参�Eを取得します、E
        /// @note こ�EメソチE��はライブラリ冁E���E�ELL墁E��の冁E�E�E�でのみ使用されます、E
        /// @return Entity を管琁E��めEunique_ptr への参�E、E
        [[nodiscard]]
        auto GetEntity() -> std::unique_ptr<Entity>&
        {
            return m_Entity;
        }
#endif

    private:
#pragma warning(push)
#pragma warning(disable : 4251) // DLL墁E��を越える際の unique_ptr に関する警告を抑制
        std::unique_ptr<Entity> m_Entity;
#pragma warning(pop)
    };

} // namespace N503::Renderer2D
#pragma once

namespace N503::Renderer2D
{
    class Text
    {
    public:
        Text();
        ~Text();
    };
} // namespace N503::Renderer2D
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>

namespace N503::Renderer2D
{

    struct RectF
    {
        float X;

        float Y;

        float Width;

        float Height;
    };

    struct PointF
    {
        float X;

        float Y;
    };

    struct SizeF
    {
        float Width;

        float Height;
    };

    struct ColorF
    {
        float Red;

        float Green;

        float Blue;

        float Alpha;
    };

    struct Transform
    {
        float ScaleX;

        float ScaleY;

        float Rotation;

        float TranslationX;

        float TranslationY;
    };

    enum class PixelFormat
    {
        Unknown = 0,

        B8G8R8A8_UNORM,
    };

    // struct Vertex
    //{
    //     PointF Position;

    //    ColorF Color;

    //    PointF TextureCoord;
    //};

    // struct Sprite
    //{
    //     RectF TargetRect;

    //    RectF SourceRect;

    //    ColorF Color;
    //};

    namespace Handle
    {

        enum class Generation : std::uint64_t
        {
            Default = 0,
        };

        enum class ResourceID : std::uint64_t
        {
            Invalid = std::uint64_t(-1),
        };

        enum class ResourceType : std::uint8_t
        {
            None = 0,
            Bitmap,
            Movie,
            Text,
        };

        enum class CommandID : std::uint64_t
        {
            Invalid = std::uint64_t(-1),
        };

        enum class CommandType : std::uint8_t
        {
            None = 0,
            Draw,
            System,
        };

    } // namespace Handle

    struct ResourceHandle
    {
        Handle::ResourceID ID{ Handle::ResourceID::Invalid };

        Handle::ResourceType Type;

        Handle::Generation Generation;

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ID != Handle::ResourceID::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ID == Handle::ResourceID::Invalid;
        }

        [[nodiscard]]
        bool operator==(const ResourceHandle&) const = default;
    };

    struct CommandHandle
    {
        Handle::CommandID ID{ Handle::CommandID::Invalid };

        Handle::CommandType Type;

        Handle::Generation Generation;

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ID != Handle::CommandID::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ID == Handle::CommandID::Invalid;
        }

        [[nodiscard]]
        bool operator==(const CommandHandle&) const = default;
    };

} // namespace N503::Renderer2D
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Codec
{

} // namespace N503::Renderer2D::Codec
#include "Pch.hpp"
#include "WicImageDecoder.hpp"

// 1. Project Headers
#include "../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <wincodec.h>

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>

namespace N503::Renderer2D::Codec
{

    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    WicImageDecoder::WicImageDecoder(const std::string_view path) : m_Path(path)
    {
        auto factory = wil::CoCreateInstance<IWICImagingFactory>(CLSID_WICImagingFactory);

        // チE��ーダーを作�Eして、画像ファイルを読み込む
        THROW_IF_FAILED(factory->CreateDecoderFromFilename(TranscodeUtf8ToWide(path).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &m_Decoder)
        );
    }

    auto WicImageDecoder::Decode(std::function<std::span<std::byte>(std::size_t)> allocate) -> Pixels::Buffer
    {
        // 画像�E最初�Eフレームを取得すめE
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        THROW_IF_FAILED(m_Decoder->GetFrame(0, &frame));

        // フォーマットコンバ�Eターを作�Eして、フレームめE2bppPBGRA形式に変換する
        auto factory = wil::CoCreateInstance<IWICImagingFactory>(CLSID_WICImagingFactory);
        wil::com_ptr<IWICFormatConverter> converter;
        THROW_IF_FAILED(factory->CreateFormatConverter(&converter));

        // 32bppPBGRA形式に変換する
        THROW_IF_FAILED(converter->Initialize(frame.get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom));

        // 画像�E幁E��E��さ、ピチE��を取得して、忁E��なバイト数を計算すめE
        const std::uint32_t width          = GetWidth();
        const std::uint32_t height         = GetHeight();
        const std::uint32_t pitch          = GetPitch();
        const std::size_t totalBytesNeeded = static_cast<std::size_t>(pitch) * height;

        // バッファを割り当てめE
        auto target = allocate(totalBytesNeeded);

        if (target.empty() || target.size() < totalBytesNeeded)
        {
            return {};
        }

        // 変換されたピクセルチE�EタをバチE��ァにコピ�Eする
        THROW_IF_FAILED(converter->CopyPixels(nullptr, pitch, static_cast<UINT>(target.size()), reinterpret_cast<BYTE*>(target.data())));

        // バッファを返す
        return {
            .Bytes  = target.data(),
            .Size   = totalBytesNeeded,
            .Width  = GetWidth(),
            .Height = GetHeight(),
            .Pitch  = pitch,
        };
    }

    auto WicImageDecoder::GetWidth() const -> std::uint32_t
    {
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        m_Decoder->GetFrame(0, &frame);

        UINT width  = 0;
        UINT height = 0;

        frame->GetSize(&width, &height);
        return width;
    }

    auto WicImageDecoder::GetHeight() const -> std::uint32_t
    {
        wil::com_ptr<IWICBitmapFrameDecode> frame;
        m_Decoder->GetFrame(0, &frame);

        UINT width  = 0;
        UINT height = 0;

        frame->GetSize(&width, &height);
        return height;
    }

    auto WicImageDecoder::GetPitch() const -> std::uint32_t
    {
        return GetWidth() * 4;
    }

    auto WicImageDecoder::GetPath() const -> std::string_view
    {
        return m_Path;
    }

} // namespace N503::Renderer2D::Codec
#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <wincodec.h>

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>

namespace N503::Renderer2D::Codec
{

    class WicImageDecoder final
    {
    public:
        WicImageDecoder(const std::string_view path);

        auto Decode(std::function<std::span<std::byte>(std::size_t)> allocate) -> Pixels::Buffer;

        auto GetWidth() const -> std::uint32_t;

        auto GetHeight() const -> std::uint32_t;

        auto GetPitch() const -> std::uint32_t;

        auto GetPath() const -> std::string_view;

    private:
        wil::com_ptr<IWICBitmapDecoder> m_Decoder;

        std::string m_Path;
    };

} // namespace N503::Renderer2D::Codec
#include "Pch.hpp"
#include "DrawBitmap.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Resource/Container.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device::Commands
{

    auto DrawBitmap::Execute(Context& context) -> void
    {
        auto d2dContext = context.GetD2DContext();

        if (!Bitmap)
        {
            auto entry = Engine::GetInstance().GetResourceContainer().Get(ResourceHandle);

            if (!entry)
            {
                return;
            }

            d2dContext->CreateBitmap(
                D2D1::SizeU(entry->Pixels.Width, entry->Pixels.Height),
                entry->Pixels.Bytes,
                entry->Pixels.Pitch,
                D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_NONE, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
                &Bitmap
            );

            if (!Bitmap)
            {
                return;
            }
        }

        d2dContext->DrawBitmap(
            Bitmap.get(),
            D2D1::RectF(0.0f, 0.0f, static_cast<FLOAT>(Bitmap->GetPixelSize().width), static_cast<FLOAT>(Bitmap->GetPixelSize().height)),
            1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        );
    }

} // namespace N503::Renderer2D::Device::Commands
#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device::Commands
{

    struct DrawBitmap
    {
        ResourceHandle ResourceHandle{};

        wil::com_ptr<ID2D1Bitmap1> Bitmap{ nullptr };

        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
#include "Pch.hpp"
#include "DrawLine.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Resource/Container.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device::Commands
{

    auto DrawLine::Execute(Context& context) -> void
    {
    }

} // namespace N503::Renderer2D::Device::Commands
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device::Commands
{

    struct DrawLine
    {
        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
#include "Pch.hpp"
#include "DrawRectangle.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Resource/Container.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device::Commands
{

    auto DrawRectangle::Execute(Context& context) -> void
    {
    }

} // namespace N503::Renderer2D::Device::Commands
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device::Commands
{

    struct DrawRectangle
    {
        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
#include "Pch.hpp"
#include "DrawText.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Engine.hpp"
#include "../../Resource/Container.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>

// 6. C++ Standard Libraries

#undef DrawText

namespace N503::Renderer2D::Device::Commands
{

    auto DrawText::Execute(Context& context) -> void
    {
    }

} // namespace N503::Renderer2D::Device::Commands
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

#undef DrawText

namespace N503::Renderer2D::Device::Commands
{
    struct DrawText
    {
        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
#pragma once

// 1. Project Headers
#include "Commands/DrawBitmap.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Device
{

    using namespace Commands;

    using Command = std::variant<std::monostate, DrawBitmap>;

} // namespace N503::Renderer2D::Device
#include "Pch.hpp"
#include "CommandList.hpp"

// 1. Project Headers
#include "Command.hpp"
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <type_traits>
#include <utility>
#include <variant>

namespace N503::Renderer2D::Device
{

    CommandList::CommandList()
    {
        Clear();
    }

    auto CommandList::Add(Command&& command) -> CommandHandle
    {
        // 空きスロチE��がなぁE��合�E無効なハンドルを返します、E
        if (m_AvailableHandles.empty())
        {
            return { .ID = Handle::CommandID::Invalid };
        }

        // 利用可能なハンドルのリストから末尾を取得します、E
        CommandHandle handle = m_AvailableHandles.back();
        m_AvailableHandles.pop_back();

        const auto index = static_cast<std::uint64_t>(handle.ID);

        m_Entries[index] = Entry{
            .Handle  = handle,
            .Command = std::move(command),
        };

        m_ActiveCount++;

        return handle;
    }

    auto CommandList::Remove(const CommandHandle handle) -> bool
    {
        const auto index = static_cast<std::uint64_t>(handle.ID);

        if (index >= MaxEntries)
        {
            return false;
        }

        // 世代チェチE��: すでに削除されてぁE��か、別のコマンドに再利用されてぁE��場合�E無視します、E
        if (m_Entries[index].Handle.Generation != handle.Generation)
        {
            return false;
        }

        // コマンドを破棁E��Etd::monostate へ戻す！E
        m_Entries[index].Command = std::monostate{};

        // 次の世代のハンドルを作�Eしてフリーリストに戻します、E
        CommandHandle nextHandle = handle;
        nextHandle.Generation    = static_cast<Handle::Generation>(static_cast<std::uint64_t>(handle.Generation) + 1);

        m_AvailableHandles.push_back(nextHandle);
        m_ActiveCount--;

        return true;
    }

    auto CommandList::Execute(Context& context) -> bool
    {
        if (m_ActiveCount == 0)
        {
            return false;
        }

        for (auto& entry : m_Entries)
        {
            // std::monostate (index 0) ではなぁE��効なコマンド�Eみ実行します、E
            if (entry.Command.index() != 0)
            {
                std::visit(
                    [&context](auto& command)
                    {
                        // std::monostate の場合�E何もしなぁE��型安�Eのためのコンパイル時�E岐！E
                        if constexpr (!std::is_same_v<std::decay_t<decltype(command)>, std::monostate>)
                        {
                            command.Execute(context);
                        }
                    },
                    entry.Command
                );
            }
        }

        return true;
    }

    auto CommandList::Clear() -> void
    {
        m_ActiveCount = 0;
        m_Entries.fill({ .Command = std::monostate{} });
        m_AvailableHandles.clear();

        // 0番から頁E��初期ハンドルをスタチE��に積みます（送E��E��積�Eことで0番から使われるよぁE��します！E
        for (std::int64_t i = MaxEntries - 1; i >= 0; --i)
        {
            m_AvailableHandles.push_back({
                .ID         = static_cast<Handle::CommandID>(i),
                .Type       = Handle::CommandType::Draw,
                .Generation = Handle::Generation::Default,
            });
        }
    }

} // namespace N503::Renderer2D::Device
#pragma once

// 1. Project Headers
#include "Command.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <cstdint>
#include <vector>

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device
{
    class CommandList
    {
        static constexpr auto MaxEntries = 1024;

    public:
        CommandList();

        // コマンドを追加し、ハンドルを返します、E
        auto Add(Command&& command) -> CommandHandle;

        // ハンドルを指定してコマンドを削除します、E
        auto Remove(const CommandHandle handle) -> bool;

        // 現在登録されてぁE��すべてのコマンドを実行します、E
        auto Execute(Context& context) -> bool;

        // すべてのコマンドをクリアし、�E期状態に戻します、E
        auto Clear() -> void;

        auto GetCount() const noexcept -> std::size_t
        {
            return m_ActiveCount;
        }

    private:
        struct Entry
        {
            CommandHandle Handle;

            Command Command;
        };

        std::array<Entry, MaxEntries> m_Entries{};

        std::vector<CommandHandle> m_AvailableHandles{};

        std::size_t m_ActiveCount{ 0 };
    };

} // namespace N503::Renderer2D::Device
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
        // Direct2D Factory のリソースを作�E
        D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        THROW_IF_FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, m_D2DFactory.put_void()));

        // DirectWrite Factory のリソースを作�E
        wil::com_ptr<IUnknown> dwriteFactory;
        THROW_IF_FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), dwriteFactory.put()));
        m_DWriteFactory = dwriteFactory.query<IDWriteFactory>();

        // Direct3D11のリソースを作�Eする
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

        // Direct3D11のリソースからDirect2Dのリソースを作�Eする
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
            // 成功したので旗を下ろぁE
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
            // チE��イスロスト発生。外部に通知してコンチE��ストを再構築させる
        }

        return hr;
    }

    auto Context::SetTarget(const RenderTarget& renderTarget) const noexcept -> void
    {
        m_D2DContext->SetTarget(renderTarget.GetTargetBitmap().get());
    }

} // namespace N503::Renderer2D::Device
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
#include <windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class RenderTarget;
}

namespace N503::Renderer2D::Device
{

    class Context final
    {
    public:
        Context();

    public:
        auto GetRenderTargetWindow() const noexcept -> HWND;

        auto BeginDraw() const noexcept -> bool;

        auto EndDraw() const noexcept -> HRESULT;

        auto SetTarget(const RenderTarget& renderTarget) const noexcept -> void;

    public:
        auto GetD3DDevice() const noexcept -> wil::com_ptr<ID3D11Device>
        {
            return m_D3DDevice;
        }

        auto GetD2DContext() const noexcept -> wil::com_ptr<ID2D1DeviceContext>
        {
            return m_D2DContext;
        }

    private:
        wil::com_ptr<ID2D1Factory1> m_D2DFactory;
        wil::com_ptr<IDWriteFactory> m_DWriteFactory;

        wil::com_ptr<ID3D11Device> m_D3DDevice;
        wil::com_ptr<ID3D11DeviceContext> m_D3DContext;

        wil::com_ptr<ID2D1Device> m_D2DDevice;
        wil::com_ptr<ID2D1DeviceContext> m_D2DContext;
    };

} // namespace N503::Renderer2D::Device
#include "Pch.hpp"
#include "RenderTarget.hpp"

// 1. Project Headers
#include "Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{

    RenderTarget::RenderTarget(const Context* context, HWND hwnd) : m_TargetWindow(hwnd)
    {
        auto d3dDevice  = context->GetD3DDevice();
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

        THROW_IF_FAILED(context->GetD2DContext()->CreateBitmapFromDxgiSurface(backBuffer.get(), &bitmapProperties, m_TargetBitmap.put()));
    }

    auto RenderTarget::Present() const noexcept -> HRESULT
    {
        return m_SwapChain->Present(1, 0);
    }

} // namespace N503::Renderer2D::Device
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
#include "Pch.hpp"
#include "AddCommandList.hpp"

// 1. Project Headers
#include "../../Device/Command.hpp"
#include "../../Device/CommandList.hpp"
#include "../../Device/Commands/DrawBitmap.hpp"
#include "../../Message/Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto AddCommandList::operator()(Message::Context& context) const -> void
    {
        if (auto handle = context.CommandList.Add(Device::Commands::DrawBitmap{ .ResourceHandle = ResourceHandle }))
        {
            *Result = handle;
        }
    }

} // namespace N503::Renderer2D::Message::Packets
#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct AddCommandList
    {
        CommandHandle* Result{};

        ResourceHandle ResourceHandle{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
#include "Pch.hpp"
#include "CreateBitmap.hpp"

// 1. Project Headers
#include "../../Codec/WicImageDecoder.hpp"
#include "../../Engine.hpp"
#include "../../Message/Context.hpp"
#include "../../Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <cstddef>
#include <span>

namespace N503::Renderer2D::Message::Packets
{

    auto CreateBitmap::operator()(Context& context) const -> void
    {
        if (Path.empty())
        {
            return;
        }

        Codec::WicImageDecoder decoder(Path);

        Pixels::Buffer pixels;
        pixels.Width  = decoder.GetWidth();
        pixels.Height = decoder.GetHeight();
        pixels.Pitch  = decoder.GetPitch();
        pixels.Bytes  = nullptr;
        pixels.Size   = static_cast<std::size_t>(pixels.Height) * pixels.Pitch;

        auto handle = Engine::GetInstance().GetResourceContainer().Add(Path, pixels); // 書き込み可能なpixels.Bytesを設宁E

        if (!handle)
        {
            return;
        }

        decoder.Decode([&pixels](auto size) { return std::span<std::byte>(pixels.Bytes, std::min(size, pixels.Size)); });

        *Result = handle;
        *Pixels = pixels;
    }

} // namespace N503::Renderer2D::Message::Packets
#pragma once

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Pixels/Buffer.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string_view>

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct CreateBitmap
    {
        ResourceHandle* Result{ nullptr };

        Pixels::Buffer* Pixels{ nullptr };

        std::string_view Path{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
#include "Pch.hpp"
#include "DeleteBitmap.hpp"

// 1. Project Headers
#include "../../Message/Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto DeleteBitmap::operator()(Context& context) const -> void
    {
    }

} // namespace N503::Renderer2D::Message::Packets
#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct DeleteBitmap
    {
        ResourceHandle ResourceHandle{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
#include "Pch.hpp"
#include "UpdateBitmapOpacity.hpp"

// 1. Project Headers
#include "../../Message/Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto UpdateBitmapOpacity::operator()(Context& context) const -> void
    {
    }

} // namespace N503::Renderer2D::Message::Packets
#pragma once

// 1. Project Headers
#include "../../Device/Context.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct UpdateBitmapOpacity
    {
        CommandHandle CommandHandle{};

        float Opacity{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
#include "Pch.hpp"
#include "UpdateBitmapPoint.hpp"

// 1. Project Headers
#include "../../Message/Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto UpdateBitmapPoint::operator()(Context& context) const -> void
    {
    }

} // namespace N503::Renderer2D::Message::Packets
#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message::Packets
{

    struct UpdateBitmapPoint
    {
        CommandHandle CommandHandle{};

        PointF Point{};

        auto operator()(Context& context) const -> void;
    };

} // namespace N503::Renderer2D::Message::Packets
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    class Queue;
}

namespace N503::Renderer2D::Device
{
    class Context;
    class CommandList;
} // namespace N503::Renderer2D::Device

namespace N503::Renderer2D::Resource
{
    class Container;
}

namespace N503::Renderer2D::Message
{

    struct Context
    {
        Message::Queue& MessageQueue;

        Device::Context& DeviceContext;

        Device::CommandList& CommandList;

        Resource::Container& ResourceContainer;
    };

} // namespace N503::Renderer2D::Message
#include "Pch.hpp"
#include "Dispatcher.hpp"

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Context.hpp"
#include "Envelope.hpp"
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Message
{

    auto Dispatcher::Dispatch(Context& context) -> void
    {
        auto envelopes = context.MessageQueue.PopAll();

        while (!envelopes.empty())
        {
            auto& message = envelopes.front();

            auto cleanup = wil::scope_exit(
                [&message]()
                {
                    if (message.Signal)
                    {
                        message.Signal->release();
                    }
                }
            );

            std::visit([&context](auto&& concrete) { concrete(context); }, message.Packet);

            envelopes.pop();
        }
    }

} // namespace N503::Renderer2D::Message
#pragma once

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message
{
    struct Context;
}

namespace N503::Renderer2D::Message
{

    class Dispatcher final
    {
    public:
        auto Dispatch(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Message
#pragma once

// 1. Project Headers
#include "Packet.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <semaphore>

namespace N503::Renderer2D::Message
{

    struct Envelope
    {
        Packet Packet;

        std::binary_semaphore* Signal;
    };

} // namespace N503::Renderer2D::Message
#pragma once

// 1. Project Headers
#include "Packets/AddCommandList.hpp"
#include "Packets/CreateBitmap.hpp"
#include "Packets/DeleteBitmap.hpp"
#include "Packets/UpdateBitmapOpacity.hpp"
#include "Packets/UpdateBitmapPoint.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Message
{

    using namespace Packets;

    using Packet = std::variant<CreateBitmap, DeleteBitmap, UpdateBitmapPoint, UpdateBitmapOpacity, AddCommandList>;

} // namespace N503::Renderer2D::Message
#include "Pch.hpp"
#include "Queue.hpp"

// 1. Project Headers
#include "Envelope.hpp"
#include "Packet.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <semaphore>
#include <utility>

namespace N503::Renderer2D::Message
{

    auto Queue::Enqueue(Packet&& packet) -> void
    {
        bool isBusy = false;

#ifdef _DEBUG
        {
            const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t capacity    = MaxMessageQueued; // プ�Eルのサイズ

            if (currentSize >= capacity * 0.95) // 95%を趁E��たら警呁E
            {
                ::OutputDebugStringW(L"EventQueue is congesting!\n");
            }
        }
#endif
        {
            const std::lock_guard lock{ m_Mutex };

            const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t capacity    = MaxMessageQueued; // プ�Eルのサイズ

            if (currentSize >= capacity * 0.95)
            {
                isBusy = true;
            }
            else
            {
                m_Buffer[m_BufferIndex].Container.push(Envelope{ .Packet = std::move(packet), .Signal = nullptr });
            }
        }

        if (isBusy)
        {
            EnqueueSync(std::move(packet));
        }
        else
        {
            m_WakeupEvent.SetEvent();
        }
    }

    auto Queue::EnqueueSync(Packet&& packet) -> void
    {
        std::binary_semaphore signal{ 0 };

        {
            const std::lock_guard lock{ m_Mutex };

            m_Buffer[m_BufferIndex].Container.push(Envelope{ .Packet = std::move(packet), .Signal = &signal });
        }

        m_WakeupEvent.SetEvent();

        signal.acquire();
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::PopAll() -> Container
    {
        const std::lock_guard lock{ m_Mutex };

        // UIスレチE��で溜めたデータを「Allocatorごと」奪ぁE��めE
        auto container = std::move(m_Buffer[m_BufferIndex].Container);

        // 空になった場所に、正しい Allocator�E�Etorageへのポインタ�E�を再裁E��
        m_Buffer[m_BufferIndex].Container = Container{ m_Buffer[m_BufferIndex].Allocator };

        // 次の Push 用にインチE��クスを�Eり替える
        m_BufferIndex = (m_BufferIndex + 1) % m_Buffer.size();

        return container;
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::GetWakeupEventHandle() const -> HANDLE
    {
        return m_WakeupEvent.get();
    }

} // namespace N503::Renderer2D::Message
#pragma once

// 1. Project Headers
#include "Envelope.hpp"
#include "Packet.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Pool.hpp>
#include <N503/Memory/StorageAllocator.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <list>
#include <mutex>
#include <queue>
#include <utility>
#include <variant>

namespace N503::Renderer2D::Message
{

    class Queue final
    {
        static const std::size_t MaxMessageQueued = 4096;

        static const std::size_t BufferCount = 2;

    public:
        using Element = Envelope;

        struct Slot
        {
            alignas(void*) std::byte data[sizeof(Element) + 32];
        };

        using Storage = ::N503::Memory::Storage::Pool<Slot>;

        using Allocator = ::N503::Memory::StorageAllocator<Element, Storage>;

        using Strategy = std::list<Element, Allocator>;

        using Container = std::queue<Element, Strategy>;

    public:
        template <typename TPacket> class MessageBorrower
        {
        public:
            MessageBorrower(std::unique_lock<std::mutex>&& lock, Container& queue) : m_Lock(std::move(lock))
            {
                if (!queue.empty() && std::holds_alternative<TPacket>(queue.back().Packet))
                {
                    m_Target = &std::get_if<TPacket>(queue.back().Packet);
                }
            }

            MessageBorrower(const MessageBorrower&) = delete;

            auto operator=(const MessageBorrower&) -> MessageBorrower& = delete;

            auto operator->() -> TPacket*
            {
                return m_Target;
            }

            explicit operator bool() const
            {
                return m_Target != nullptr;
            }

        private:
            std::unique_lock<std::mutex> m_Lock;

            TPacket* m_Target = nullptr;
        };

    public:
        Queue() = default;

        Queue(const Queue&) = delete;

        auto operator=(const Queue&) -> Queue& = delete;

        Queue(Queue&&) = delete;

        auto operator=(Queue&&) -> Queue& = delete;

    public:
        auto Enqueue(Packet&&) -> void;

        auto EnqueueSync(Packet&&) -> void;

        [[nodiscard]]
        auto PopAll() -> Container;

        [[nodiscard]]
        auto GetWakeupEventHandle() const -> HANDLE;

    public:
        template <typename TDataType> auto TryBorrowBack() -> MessageBorrower<TDataType>
        {
            // ロチE��を奪取すめE
            std::unique_lock<std::mutex> lock{ m_Mutex };

            // ロチE��した状態で Container を特定し、lock ごと Borrower に渡ぁE
            return MessageBorrower<TDataType>{ std::move(lock), m_Buffer[m_BufferIndex].Container };
        }

    private:
        /// @brief
        struct Buffer
        {
            typename Queue::Storage Storage{ MaxMessageQueued }; ///< Allocator より先に宣言する忁E��がある事に注愁E

            typename Queue::Allocator Allocator{ &Storage }; ///< Container より先に宣言する忁E��がある事に注愁E

            typename Queue::Container Container{ Allocator };
        };

        std::array<Buffer, BufferCount> m_Buffer{};

        std::uint32_t m_BufferIndex{ 0 };

        wil::unique_event_nothrow m_WakeupEvent{ ::CreateEventW(nullptr, FALSE, FALSE, nullptr) };

        std::mutex m_Mutex;
    };

} // namespace N503::Renderer2D::Message
#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>

namespace N503::Renderer2D::Pixels
{

    struct Buffer
    {
        std::byte* Bytes{ nullptr };

        std::size_t Size{ 0 };

        std::uint32_t Width{ 0 };

        std::uint32_t Height{ 0 };

        std::uint32_t Pitch{ 0 };

        PixelFormat Format{ PixelFormat::Unknown };
    };

} // namespace N503::Renderer2D::Pixels
#include "Pch.hpp"
#include "Container.hpp"

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace N503::Renderer2D::Resource
{

    Container::Container()
    {
        Clear();
    }

    auto Container::Add(const std::string_view path, Pixels::Buffer& pixels) -> ResourceHandle
    {
        // すでに同じパスのリソースが存在する場合�E、そのハンドルを返します、E
        if (auto it = m_Indexes.find(path); it != m_Indexes.end())
        {
            return it->second;
        }

        // 利用可能なハンドルがなぁE��合�E、無効なハンドルを返します、E
        if (m_AvailableHandles.empty())
        {
            return { .ID = Handle::ResourceID::Invalid };
        }

        // 利用可能なハンドルのリストからハンドルを取得します、E
        ResourceHandle handle = m_AvailableHandles.back();
        m_AvailableHandles.pop_back();

        // ハンドルのIDをインチE��クスとして使用します。これにより、O(1)でエントリにアクセスできます、E
        const auto index = static_cast<std::uint64_t>(handle.ID);

        // 画像データのバイトサイズに基づぁE��、アリメントを16バイトとした生�Eバイト�Eとしてメモリを確保します、E
        void* address = m_Storage.AllocateBytes(pixels.Size, 16);

        // 呼び出し�Eに書き込み可能なバッファの位置を伝えるため、Pixels構造体�EBytesにアドレスをセチE��します、E
        pixels.Bytes = static_cast<std::byte*>(address);

        // エントリを作�Eして保存します、E
        m_Entries[index] = Entry{
            .Handle   = handle,
            .Pixels   = pixels,
            .Metadata = { .Path = std::string(path) },
        };

        // パスとハンドルの対応を保存します、E
        m_Indexes[m_Entries[index].Metadata.Path] = handle;

        return handle;
    }

    auto Container::Remove(const ResourceHandle handle) -> bool
    {
        // ハンドルのIDをインチE��クスとして使用します、E
        const auto index = static_cast<std::uint64_t>(handle.ID);

        if (index >= MaxEntries)
        {
            return false;
        }

        // ハンドルの世代がエントリの世代と一致しなぁE��合�E、すでに無効であるか、あるいは世代が古ぁE��みなします、E
        if (m_Entries[index].Handle.Generation != handle.Generation)
        {
            return false; // すでに無効、あるいは世代が古ぁE
        }

        // パスとハンドルの対応を削除します、E
        m_Indexes.erase(m_Entries[index].Metadata.Path);

        // エントリを空にして、次の世代のハンドルを利用可能なハンドルのリストに追加します、E
        ResourceHandle nextGenerationHandle = handle;
        nextGenerationHandle.Generation     = static_cast<Handle::Generation>(static_cast<std::uint64_t>(handle.Generation) + 1);

        // エントリを空にします、E
        m_Entries[index] = {};

        // 次の世代のハンドルを利用可能なハンドルのリストに追加します、E
        m_AvailableHandles.push_back(nextGenerationHandle);

        return true;
    }

    auto Container::Get(const ResourceHandle handle) const -> const Entry*
    {
        const auto index = static_cast<std::uint64_t>(handle.ID);

        if (index >= MaxEntries)
        {
            return nullptr;
        }

        if (m_Entries[index].Handle.Generation != handle.Generation)
        {
            return nullptr; // 無効、あるいは世代が古ぁE
        }

        return &m_Entries[index];
    }

    auto Container::Get(const std::string_view path) const -> const Entry*
    {
        if (auto it = m_Indexes.find(path); it != m_Indexes.end())
        {
            return Get(it->second);
        }

        return nullptr;
    }

    auto Container::GetHandle(const std::string_view path) const -> ResourceHandle
    {
        if (auto it = m_Indexes.find(path); it != m_Indexes.end())
        {
            return it->second;
        }

        return { .ID = Handle::ResourceID::Invalid };
    }

    auto Container::Clear() -> void
    {
        // すべてのエントリをクリアし、利用可能なハンドルのリストを初期化します、E
        m_Indexes.clear();
        m_Entries.fill({});
        m_AvailableHandles.clear();

        // Arenaの状態をリセチE��します。これにより、すべての確保されたメモリが解放され、次の確保�E最初�EブロチE��から始まります、E
        m_Storage.Reset();

        // 利用可能なハンドルのリストを初期化します。これにより、最初�EMaxEntries個�EリソースIDが利用可能になります、E
        for (std::uint64_t i = 0; i < MaxEntries; ++i)
        {
            m_AvailableHandles.push_back({ .ID = static_cast<Handle::ResourceID>(i), .Generation = Handle::Generation::Default });
        }
    }

} // namespace N503::Renderer2D::Resource
#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Arena.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace N503::Renderer2D::Resource
{

    class Container
    {
        static constexpr auto MaxEntries = 1024;

    public:
        Container();

        auto Add(const std::string_view path, Pixels::Buffer& pixels) -> ResourceHandle;

        auto Remove(const ResourceHandle handle) -> bool;

        auto Get(const ResourceHandle handle) const -> const Entry*;

        auto Get(const std::string_view path) const -> const Entry*;

        auto GetHandle(const std::string_view path) const -> ResourceHandle;

        auto Clear() -> void;

    private:
        N503::Memory::Storage::Arena m_Storage{ MaxEntries * 32768 };

        std::array<Entry, MaxEntries> m_Entries;

        std::vector<ResourceHandle> m_AvailableHandles;

        std::unordered_map<std::string_view, ResourceHandle, std::hash<std::string_view>, std::equal_to<>> m_Indexes;
    };

} // namespace N503::Renderer2D::Resource
#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Metadata.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Renderer2D::Resource
{

    struct Entry
    {
        ResourceHandle Handle;

        Pixels::Buffer Pixels;

        Metadata Metadata;
    };

} // namespace N503::Renderer2D::Resource
#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Renderer2D::Resource
{

    struct Metadata
    {
        std::string Path;
    };

} // namespace N503::Renderer2D::Resource
#include "Pch.hpp"
#include "Bitmap_Entity.hpp"

// 1. Project Headers
#include "Engine.hpp"
#include "Message/Packets/AddCommandList.hpp"
#include "Message/Packets/CreateBitmap.hpp"
#include "Message/Packets/UpdateBitmapOpacity.hpp"
#include "Message/Packets/UpdateBitmapPoint.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Bitmap.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>
#include <utility>

namespace N503::Renderer2D
{

    Bitmap::Bitmap(const std::string_view path)
    {
        m_Entity = std::make_unique<Entity>();

        auto packet = Message::Packets::CreateBitmap{
            .Result = &m_Entity->ResourceHandle,
            .Pixels = &m_Entity->Pixels,
            .Path   = path,
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

        auto packet2 = Message::Packets::AddCommandList{
            .Result         = &m_Entity->CommandHandle,
            .ResourceHandle = m_Entity->ResourceHandle,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet2));
    }

    auto Bitmap::SetPoint(float x, float y) -> void
    {
        if (m_Entity == nullptr)
        {
            return;
        }

        auto packet = Message::Packets::UpdateBitmapPoint{
            .CommandHandle = m_Entity->CommandHandle,
            .Point         = PointF{ x, y },
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Bitmap::SetOpacity(float opacity) -> void
    {
        if (m_Entity == nullptr)
        {
            return;
        }

        auto packet = Message::Packets::UpdateBitmapOpacity{
            .CommandHandle = m_Entity->CommandHandle,
            .Opacity       = opacity,
        };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Bitmap::GetWidth() const -> float
    {
        if (m_Entity == nullptr)
        {
            return 0.0f;
        }

        return m_Entity->Pixels.Width;
    }

    auto Bitmap::GetHeight() const -> float
    {
        if (m_Entity == nullptr)
        {
            return 0.0f;
        }

        return m_Entity->Pixels.Height;
    }

    auto Bitmap::GetPitch() const -> float
    {
        if (m_Entity == nullptr)
        {
            return 0.0f;
        }

        return m_Entity->Pixels.Pitch;
    }

    Bitmap::~Bitmap() = default;

    Bitmap::Bitmap(Bitmap&& other) = default;

    auto Bitmap::operator=(Bitmap&& other) -> Bitmap& = default;

} // namespace N503::Renderer2D
#pragma once

// 1. Project Headers
#include "Pixels/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <N503/Renderer2D/Bitmap.hpp>
#include <N503/Renderer2D/Types.hpp>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D
{

    struct Bitmap::Entity
    {
        ResourceHandle ResourceHandle{};

        CommandHandle CommandHandle{};

        Pixels::Buffer Pixels{};
    };

} // namespace N503::Renderer2D
#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Device/CommandList.hpp"
#include "Device/Context.hpp"
#include "Device/RenderTarget.hpp"
#include "Message/Context.hpp"
#include "Message/Dispatcher.hpp"
#include "Message/Queue.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <format>
#include <memory>
#include <semaphore>
#include <thread>
#include <utility>

namespace N503::Renderer2D
{

    auto Engine::GetInstance() noexcept -> Engine&
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine()
    {
        // リソース コンチE��を�E期化する
        m_ResourceContainer = std::make_unique<Resource::Container>();
        // メチE��ージ キューを�E期化する
        m_MessageQueue = std::make_unique<Message::Queue>();
    }

    auto Engine::Start() -> void
    {
        // すでにエンジンが開始されてぁE��場合�E、何もしなぁE
        if (m_IsRunning.load(std::memory_order_acquire))
        {
            return;
        }

        std::binary_semaphore signal{ 0 };

        // レンダラー スレチE��を作�Eする
        m_RendererThread = std::jthread(
            [this, &signal](const std::stop_token stopToken)
            {
                // スレチE��名を設定すめE
                ::SetThreadDescription(::GetCurrentThread(), L"N503.CppWin32.Renderer2D");

                // レンダラー スレチE��の初期化が完亁E��たことを通知する
                signal.release();

                // レンダラー スレチE��の初期化が完亁E��たことを示すイベントをシグナル状態にする
                m_StartedEvent.SetEvent();

                // COM を�E期化する
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);

                // PeekMessageWを呼び出して、レンダラースレチE��のメチE��ージキューを作�Eする
                MSG msg{};
                ::PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

                // レンダラースレチE��を開始すめE
                Run(std::move(stopToken));

                // レンダラー スレチE��が停止したことを示すイベントをリセチE��状態にする
                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        // スレチE��が起動して初期化が完亁E��るまで征E��すめE
        signal.acquire();

        // エンジンが開始されたことを示すイベントをシグナル状態にする
        m_IsRunning.store(true, std::memory_order_release);
    }

    auto Engine::Stop() -> void
    {
        if (!::PostThreadMessage(::GetThreadId(m_RendererThread.native_handle()), WM_QUIT, 0, 0))
        {
            const Diagnostics::Entry entry{
                .Severity = Diagnostics::Severity::Error,
                .Expected = std::format("PostThreadMessage failed: Reason={}, Handle={}\n", ::GetLastError(), m_RendererThread.native_handle()).data(),
                .Position = 0
            };

            m_DiagnosticsSink.AddEntry(entry);
        }
    }

    auto Engine::Wait() -> void
    {
        if (m_RendererThread.joinable())
        {
            m_RendererThread.join();
        }
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        std::unique_ptr<Device::Context> deviceContext     = std::make_unique<Device::Context>();
        std::unique_ptr<Device::RenderTarget> renderTarget = nullptr;
        std::unique_ptr<Device::CommandList> commandList   = std::make_unique<Device::CommandList>();

        // メチE��ージ チE��スパッチャーを�E期化する
        Message::Dispatcher messageDispatcher;

        // レンダラースレチE��の終亁E��にリソースをクリーンアチE�EするためのスコーチEガーチE
        auto cleanupResources = wil::scope_exit(
            [&]
            {
                renderTarget.reset();
                deviceContext.reset();
                commandList.reset();
            }
        );

        // OS メチE��ージをディスパッチするラムダ弁E
        auto OSMessageDispatch = []() -> bool
        {
            MSG message{};

            while (::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    return false;
                }

                ::TranslateMessage(&message);
                ::DispatchMessageW(&message);
            }

            return true;
        };

        // レンダラー スレチE��のメイン ルーチE
        auto waitHandles = { m_MessageQueue->GetWakeupEventHandle() };
        auto isAnyActive = false;

        while (!stopToken.stop_requested())
        {
            HWND hwnd{};

            if (renderTarget == nullptr && (hwnd = deviceContext->GetRenderTargetWindow()))
            {
                renderTarget = std::make_unique<Device::RenderTarget>(deviceContext.get(), hwnd);
                deviceContext->SetTarget(*renderTarget);
            }

            const auto count   = static_cast<DWORD>(waitHandles.size());
            const auto handles = waitHandles.begin();
            const auto timeout = isAnyActive ? 0 : INFINITE;

            auto result = ::MsgWaitForMultipleObjectsEx(count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + count))
            {
                Message::Context context{
                    .MessageQueue      = *m_MessageQueue,
                    .DeviceContext     = *deviceContext,
                    .CommandList       = *commandList,
                    .ResourceContainer = *m_ResourceContainer,
                };

                messageDispatcher.Dispatch(context);
            }
            else if (result == WAIT_OBJECT_0 + count)
            {
                if (!OSMessageDispatch())
                {
                    return;
                }
            }

            if (deviceContext->BeginDraw())
            {
                isAnyActive = commandList->Execute(*deviceContext);

                const auto endDrawResult = deviceContext->EndDraw();
                const auto presentResult = renderTarget->Present();

                if (endDrawResult == D2DERR_RECREATE_TARGET || presentResult == DXGI_ERROR_DEVICE_REMOVED || presentResult == DXGI_ERROR_DEVICE_RESET)
                {
                    // チE��イスロスト発生。リソースをクリーンアチE�EしてコンチE��ストを再構築すめE
                    deviceContext = std::make_unique<Device::Context>();
                    renderTarget  = std::make_unique<Device::RenderTarget>(deviceContext.get(), renderTarget->GetTargetWindow());
                    deviceContext->SetTarget(*renderTarget);
                }
            }
        }
    }

} // namespace N503::Renderer2D
#pragma once

// 1. Project Headers
#include "Message/Queue.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <memory>
#include <thread>

namespace N503::Renderer2D
{

    class Engine final
    {
    public:
        static Engine& GetInstance() noexcept;

    public:
        Engine();

    public:
        auto Start() -> void;

        auto Stop() -> void;

        auto Wait() -> void;

        auto Run(const std::stop_token stopToken) -> void;

    public:
        auto GetResourceContainer() const -> Resource::Container&
        {
            return *m_ResourceContainer;
        }

        auto GetMessageQueue() const -> Message::Queue&
        {
            return *m_MessageQueue;
        }

        auto GetDiagnosticsSink() const -> const Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

        auto GetDiagnosticsSink() -> Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

    private:
        std::atomic<bool> m_IsRunning{ false };

        wil::unique_event m_StartedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, L"Local\\N503.CppWin32.Renderer2D.Event.EngineStarted") };

        Diagnostics::Sink m_DiagnosticsSink;

        std::unique_ptr<Resource::Container> m_ResourceContainer;

        std::unique_ptr<Message::Queue> m_MessageQueue;

        std::jthread m_RendererThread;
    };

} // namespace N503::Renderer2D
#include "Pch.hpp"
#include "Text_Entity.hpp"

namespace N503::Renderer2D
{
} // namespace N503::Renderer2D
#pragma once

namespace N503::Renderer2D
{
} // namespace N503::Renderer2D
#include "Pch.hpp"
#pragma once

// C++/STL
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// Windows Dependencies
// #define WIN32_LEAN_AND_MEAN
// #define NOMINMAX
// #define STRICT
// #define STRICT_TYPED_ITEMIDS
#include <Windows.h>

// Windows Implementation Libraries
// #include <wil/result.h>
// #include <wil/resource.h>
// #include <wil/com.h>
// #include <wil/win32_helpers.h>
