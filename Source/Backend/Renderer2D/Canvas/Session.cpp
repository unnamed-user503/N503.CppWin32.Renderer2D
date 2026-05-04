#include "Pch.hpp"
#include "Session.hpp"

// 1. Project Headers
#include "Cache.hpp"
#include "Device.hpp"

// 5. Windows Headers
#include <d2d1_3.h>
#include <d2d1helper.h>

namespace N503::Renderer2D::Canvas
{
    Session::Session(Device* device) : m_Device(device)
    {
        // 実際の BeginDraw は Surface::Begin で実行済み[cite: 10]
    }

    Session::Session(Session&& other) noexcept : m_Device(other.m_Device), m_IsEnded(other.m_IsEnded)
    {
        other.m_Device  = nullptr;
        other.m_IsEnded = true;
    }

    auto Session::operator=(Session&& other) noexcept -> Session&
    {
        if (this != &other)
        {
            End(); // 現在のセッションを安全に終了[cite: 3, 4]
            m_Device        = other.m_Device;
            m_IsEnded       = other.m_IsEnded;
            other.m_Device  = nullptr;
            other.m_IsEnded = true;
        }
        return *this;
    }

    Session::~Session()
    {
        std::ignore = End();
    }

    auto Session::End() noexcept -> HRESULT
    {
        if (m_IsEnded || !m_Device)
        {
            return S_OK;
        }

        m_IsEnded = true;
        // Device::D2dContext() の参照経由で描画を確定[cite: 7, 8]
        HRESULT hr = m_Device->GetD2D1DeviceContext().EndDraw();

        // 次のセッションのためにターゲットをクリア[cite: 10]
        m_Device->GetD2D1DeviceContext().SetTarget(nullptr);

        return hr;
    }

    // ---- 描画命令の実装 ----

    auto Session::FillRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color) -> void
    {
        // Cache クラスの GetOrCreateBrush を使用 (D2D1_COLOR_F は ColorF と互換)
        auto brush = m_Device->GetOrCreateBrush({ color.r, color.g, color.b, color.a });
        m_Device->GetD2D1DeviceContext().FillRectangle(rect, brush.get());
    }

    auto Session::DrawRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth) -> void
    {
        auto brush = m_Device->GetOrCreateBrush({ color.r, color.g, color.b, color.a });
        m_Device->GetD2D1DeviceContext().DrawRectangle(rect, brush.get(), strokeWidth);
    }

    auto Session::DrawBitmap(ID2D1Bitmap* bitmap, const D2D1_RECT_F* destination, float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode) -> void
    {
        m_Device->GetD2D1DeviceContext().DrawBitmap(bitmap, destination, opacity, interpolationMode);
    }

    auto Session::GetDefaultSpriteBatch() const noexcept -> ID2D1SpriteBatch*
    {
        return &m_Device->GetDefaultSpriteBatch();
    }

    auto Session::AddSprites(
        ID2D1SpriteBatch* spriteBatch,
        UINT32 spriteCount,
        const D2D1_RECT_F* destinationRects,
        const D2D1_RECT_U* sourceRects,
        const D2D1_COLOR_F* colors,
        const D2D1_MATRIX_3X2_F* transforms,
        UINT32 destinationRectsStride,
        UINT32 sourceRectsStride,
        UINT32 colorsStride,
        UINT32 transformsStride
    ) -> void
    {
        if (!spriteBatch)
        {
            return;
        }

        // ID2D1SpriteBatch::AddSprites の呼び出し
        spriteBatch->AddSprites(
            spriteCount, destinationRects, sourceRects, colors, transforms, destinationRectsStride, sourceRectsStride, colorsStride, transformsStride
        );
    }

    auto Session::DrawSpriteBatch(
        ID2D1SpriteBatch* spriteBatch, ID2D1Bitmap* bitmap, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode, D2D1_SPRITE_OPTIONS spriteOptions
    ) -> void
    {
        if (!spriteBatch || !bitmap || !m_Device)
        {
            return;
        }

        // デバイスコンテキスト経由で描画を実行
        m_Device->GetD2D1DeviceContext().DrawSpriteBatch(spriteBatch, bitmap, interpolationMode, spriteOptions);
    }

    auto Session::DrawString(std::wstring_view text, std::wstring_view fontName, float fontSize, const D2D1_POINT_2F& origin, const D2D1_COLOR_F& color) -> void
    {
        // 1. テキスト形式の取得
        auto format = m_Device->GetOrCreateTextFormat(fontName, fontSize);

        // 2. ブラシの取得
        auto brush = m_Device->GetOrCreateBrush({ color.r, color.g, color.b, color.a });

        // 3. テキストレイアウトの生成 (wil::com_ptr をそのまま渡す)
        auto layout = m_Device->GetOrCreateTextLayout(text, format);

        // 4. 描画
        m_Device->GetD2D1DeviceContext().DrawTextLayout(origin, layout.get(), brush.get());
    }

    // ---- 座標変換の実装 ----

    auto Session::SetTransform(const D2D1_MATRIX_3X2_F& transform) -> void
    {
        m_Device->GetD2D1DeviceContext().SetTransform(transform);
    }

    auto Session::ResetTransform() -> void
    {
        m_Device->GetD2D1DeviceContext().SetTransform(D2D1::Matrix3x2F::Identity());
    }

    auto Session::SetAntialiasMode(D2D1_ANTIALIAS_MODE mode) noexcept -> void
    {
        if (!m_Device)
        {
            return;
        }

        // デバイスコンテキストに対してアンチエイリアスモードを適用[cite: 7]
        m_Device->GetD2D1DeviceContext().SetAntialiasMode(mode);
    }

    auto Session::GetAntialiasMode() const noexcept -> D2D1_ANTIALIAS_MODE
    {
        if (!m_Device)
        {
            return D2D1_ANTIALIAS_MODE_PER_PRIMITIVE; // デフォルト値を返却
        }

        // 現在の設定を取得[cite: 7]
        return m_Device->GetD2D1DeviceContext().GetAntialiasMode();
    }

} // namespace N503::Renderer2D::Canvas
