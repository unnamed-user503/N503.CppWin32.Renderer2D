#pragma once

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 5. Windows Headers
#include <d2d1_3.h>
#include <d3d11.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>

namespace N503::Renderer2D::Pixels
{
    struct Buffer;
}

namespace N503::Renderer2D::Canvas
{
    class Cache;

    class Device
    {
    public:
        Device();
        ~Device();

        // コピー禁止
        Device(const Device&)                    = delete;
        auto operator=(const Device&) -> Device& = delete;

        // ---- アクセッサ ----
        // 生の参照を返すことで、利用側での .get() 呼び出しを減らし、
        // かつ生存期間が Device に紐付いていることを明示します
        auto GetD3D11Device() const noexcept -> ID3D11Device&
        {
            return *m_D3DDevice.get();
        }
        auto GetD2D1DeviceContext() const noexcept -> ID2D1DeviceContext3&
        {
            return *m_D2DContext.get();
        }
        auto GetCache() noexcept -> Canvas::Cache&
        {
            return *m_ResourceCache;
        }

        // ---- リソース作成メソッド (主に Cache から呼ばれる)[cite: 7, 15] ----
        auto CreateBitmap(const Pixels::Buffer& pixels) -> wil::com_ptr<ID2D1Bitmap1>;
        auto CreateSolidColorBrush(const D2D1_COLOR_F& color) -> wil::com_ptr<ID2D1SolidColorBrush>;
        auto CreateTextFormat(std::wstring_view fontName, float fontSize) -> wil::com_ptr<IDWriteTextFormat>;
        auto CreateTextLayout(std::wstring_view text, IDWriteTextFormat* format) -> wil::com_ptr<IDWriteTextLayout>;
        auto GetDefaultSpriteBatch() -> ID2D1SpriteBatch&;

    private:
        auto InitializeDirectX() -> void;

    private:
        // 命名規則: m_PascalCase[cite: 6, 16]
        wil::com_ptr<ID3D11Device> m_D3DDevice;
        wil::com_ptr<ID3D11DeviceContext> m_D3DContext;

        wil::com_ptr<ID2D1Factory3> m_D2DFactory;
        wil::com_ptr<ID2D1Device2> m_D2DDevice;
        wil::com_ptr<ID2D1DeviceContext3> m_D2DContext;

        wil::com_ptr<IDWriteFactory> m_DWriteFactory;

        wil::com_ptr<ID2D1SpriteBatch> m_DefaultSpriteBatch;

        // リソースキャッシュ
        std::unique_ptr<Canvas::Cache> m_ResourceCache;
    };

} // namespace N503::Renderer2D::Canvas
