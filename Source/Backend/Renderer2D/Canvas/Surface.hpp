#pragma once

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 5. Windows Headers
#include <d2d1_3.h>
#include <dxgi1_2.h>

namespace N503::Renderer2D::Canvas
{
    class Device;
    class Session;

    // ウィンドウ（HWND）に関連付けられた描画ターゲットとスワップチェーンを管理する[cite: 9, 10]
    class Surface
    {
    public:
        // Device 経由で生成。内部で SwapChain を構築する
        Surface(Device& device, HWND hwnd);

        // コピー禁止
        Surface(const Surface&)                    = delete;
        auto operator=(const Surface&) -> Surface& = delete;

        // ムーブ許可
        Surface(Surface&&) noexcept                    = default;
        auto operator=(Surface&&) noexcept -> Surface& = default;

        ~Surface() = default;

    public:
        // 描画セッションを開始する。返された Session の破棄時に EndDraw が呼ばれる[cite: 5, 7]
        [[nodiscard]] auto Begin(const D2D1_COLOR_F& clearColor = { 0.0f, 0.0f, 0.0f, 1.0f }) -> Session;

        // スワップチェーンのバッファサイズを更新する[cite: 9]
        auto Resize(UINT32 width, UINT32 height) -> void;

        // 描画結果を表示する[cite: 9, 10]
        auto Present(UINT syncInterval = 1) noexcept -> HRESULT;

        // アクセサ
        auto GetTargetWindow() const noexcept -> HWND
        {
            return m_TargetWindow;
        }

        auto GetTargetBitmap() const noexcept -> ID2D1Bitmap1*
        {
            return m_TargetBitmap.get();
        }

        auto GetWidth() const noexcept -> UINT32
        {
            return m_Width;
        }

        auto GetHeight() const noexcept -> UINT32
        {
            return m_Height;
        }

    private:
        Device& m_Device;                          // 共有デバイスへの参照
        wil::com_ptr<IDXGISwapChain1> m_SwapChain; // 命名規則: m_PascalCase[cite: 6, 9]
        wil::com_ptr<ID2D1Bitmap1> m_TargetBitmap; // D2D描画ターゲット[cite: 9]
        HWND m_TargetWindow{ nullptr };
        UINT32 m_Width{ 0 };
        UINT32 m_Height{ 0 };
    };

} // namespace N503::Renderer2D::Canvas
