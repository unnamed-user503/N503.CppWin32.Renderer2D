#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>

namespace N503::Renderer2D::System
{

    static constexpr std::size_t MaxEntities = 32768;

    static constexpr std::size_t MaxComponents = 4;

    using Entity = std::uint32_t;

    [[nodiscard]]
    constexpr std::size_t ToArrayIndex(Entity entity) noexcept
    {
        return static_cast<std::size_t>(entity);
    }

    enum class ComponentType : std::size_t
    {
        Transform = 0,
        Sprite    = 1,
        Text      = 2,
    };

    enum class RenderGroup : std::uint8_t
    {
        Background = 0, // 背景（SpriteBatch推奨）
        World,          // メインのゲームオブジェクト
        Effect,         // エフェクト
        UI,             // UI（ソート不要な場合が多い）
        Limited,        // レイヤー数の上限
    };

    struct Transform
    {
        float X{ 0.0f };

        float Y{ 0.0f };

        float Z{ 0.0f };

        float Rotation{ 0.0f };

        float ScaleX{ 1.0f };

        float ScaleY{ 1.0f };

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            X        = 0.0f;
            Y        = 0.0f;
            Z        = 0.0f;
            Rotation = 0.0f;
            ScaleX   = 1.0f;
            ScaleY   = 1.0f;
            IsDirty  = true;
        }
    };

    struct Sprite
    {
        Renderer2D::ResourceHandle ResourceHandle;

        wil::com_ptr<ID2D1Bitmap1> Bitmap;

        RenderGroup Group{ RenderGroup::World };

        D2D1::Matrix3x2F Transform{ D2D1::Matrix3x2F::Identity() };

        float Width{ 0.0f };

        float Height{ 0.0f };

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            ResourceHandle = { .ID = Handle::ResourceID::Invalid, .Type = Handle::ResourceType::None, .Generation = Handle::Generation::Default };
            Bitmap.reset();
            Group     = RenderGroup::World;
            Transform = D2D1::Matrix3x2F::Identity();
            Width     = 0.0f;
            Height    = 0.0f;
            IsDirty   = true;
        }
    };

    struct SpriteBatch
    {
        Renderer2D::ResourceHandle ResourceHandle;
        wil::com_ptr<ID2D1Bitmap1> Bitmap;
        RenderGroup Group{ RenderGroup::World };

        // ID2D1SpriteBatch::AddSprites に渡すデータ形式に合わせる
        // D2D1_RECT_F: 転送先（画面上のどこか）
        // D2D1_RECT_F: 転送元（画像の中のどこか）
        // D2D1_COLOR_F: 色（透明度など）
        // D2D1_MATRIX_3X2_F: 個別の回転・拡大
        std::vector<D2D1_RECT_F> DestinationRects;
        std::vector<D2D1_RECT_F> SourceRects;
        std::vector<D2D1_COLOR_F> Colors;
        std::vector<D2D1_MATRIX_3X2_F> Transforms; // Cache the transform for each sprite to avoid redundant calculations during rendering

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            Bitmap.reset();
            DestinationRects.clear();
            SourceRects.clear();
            Colors.clear();
            Transforms.clear();
            IsDirty = true;
        }
    };

    struct Text
    {
        std::wstring Content;

        wil::com_ptr<IDWriteTextFormat> TextFormat;

        wil::com_ptr<IDWriteTextLayout> TextLayout;

        wil::com_ptr<ID2D1SolidColorBrush> Brush;

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            Content.clear();
            TextFormat.reset();
            TextLayout.reset();
            Brush.reset();
            IsDirty = false;
        }
    };

    // require:
    // auto world = std::make_unique<N503::Renderer2D::System::World>();
    struct World
    {
        std::bitset<MaxEntities> AliveBits;

        std::array<std::bitset<4>, MaxEntities> ComponentMasks;

        std::array<Transform, MaxEntities> Transforms;

        std::array<Sprite, MaxEntities> Sprites;

        std::array<Text, MaxEntities> Texts;
    };

} // namespace N503::Renderer2D::System
