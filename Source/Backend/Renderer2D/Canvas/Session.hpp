#pragma once

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 5. Windows Headers
#include <d2d1_3.h>

// 6. C++ Standard Libraries
#include <string_view>

namespace N503::Renderer2D::Canvas
{
    class Device;

    // RAIIによって BeginDraw / EndDraw のスコープを管理する描画セッション
    class Session
    {
    public:
        // Surface::Begin() からのみ生成される[cite: 5, 10]
        explicit Session(Device* device);

        // ムーブセマンティクス[cite: 4]
        Session(Session&& other) noexcept;
        auto operator=(Session&& other) noexcept -> Session&;

        // コピー禁止
        Session(const Session&)                    = delete;
        auto operator=(const Session&) -> Session& = delete;

        // デストラクタで自動的に EndDraw を呼び出し、リソースを確定させる[cite: 4]
        ~Session();

        // 描画を明示的に終了し、HRESULTを確認する[cite: 4, 7]
        auto End() noexcept -> HRESULT;

    public:
        // ---- 描画命令 (キャッシュ活用型) ----

        // キャッシュされたブラシを使用して矩形を塗りつぶす
        auto FillRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color) -> void;

        // キャッシュされたブラシを使用して矩形の枠線を描画する
        auto DrawRect(const D2D1_RECT_F& rect, const D2D1_COLOR_F& color, float strokeWidth = 1.0f) -> void;

        // 指定したビットマップを描画する[cite: 7]
        auto DrawBitmap(
            ID2D1Bitmap* bitmap,
            const D2D1_RECT_F* destination                   = nullptr,
            float opacity                                    = 1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        ) -> void;

        // デバイスが所有するデフォルトのスプライトバッチを取得する
        auto GetDefaultSpriteBatch() const noexcept -> ID2D1SpriteBatch*;

        auto AddSprites(
            ID2D1SpriteBatch* spriteBatch,
            UINT32 spriteCount,
            const D2D1_RECT_F* destinationRects,
            const D2D1_RECT_U* sourceRects      = nullptr,
            const D2D1_COLOR_F* colors          = nullptr,
            const D2D1_MATRIX_3X2_F* transforms = nullptr,
            UINT32 destinationRectsStride       = sizeof(D2D1_RECT_F),
            UINT32 sourceRectsStride            = sizeof(D2D1_RECT_F),
            UINT32 colorsStride                 = sizeof(D2D1_COLOR_F),
            UINT32 transformsStride             = sizeof(D2D1_MATRIX_3X2_F)
        ) -> void;

        // ID2D1DeviceContext3 の機能を利用した SpriteBatch 描画[cite: 7]
        auto DrawSpriteBatch(
            ID2D1SpriteBatch* spriteBatch,
            ID2D1Bitmap* bitmap,
            D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1_SPRITE_OPTIONS spriteOptions                = D2D1_SPRITE_OPTIONS_NONE
        ) -> void;

        // キャッシュされたテキスト形式とレイアウトを使用してテキストを描画する[cite: 7]
        auto DrawString(std::wstring_view text, std::wstring_view fontName, float fontSize, const D2D1_POINT_2F& origin, const D2D1_COLOR_F& color) -> void;

        // ---- 座標変換 ----

        auto SetTransform(const D2D1_MATRIX_3X2_F& transform) -> void;
        auto ResetTransform() -> void;

        // アンチエイリアスモードを設定する[cite: 7]
        auto SetAntialiasMode(D2D1_ANTIALIAS_MODE mode) noexcept -> void;

        // 現在のアンチエイリアスモードを取得する[cite: 7]
        auto GetAntialiasMode() const noexcept -> D2D1_ANTIALIAS_MODE;

    private:
        Device* m_Device; // 命名規則: m_PascalCase[cite: 6]
        bool m_IsEnded{ false };
    };

} // namespace N503::Renderer2D::Canvas
