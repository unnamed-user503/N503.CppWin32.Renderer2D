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

    enum class Entity : std::uint64_t
    {
    };

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

    struct Transform
    {
        float X{ 0.0f };

        float Y{ 0.0f };

        float Rotation{ 0.0f };

        float ScaleX{ 1.0f };

        float ScaleY{ 1.0f };

        auto Reset() noexcept -> void
        {
            X        = 0.0f;
            Y        = 0.0f;
            Rotation = 0.0f;
            ScaleX   = 1.0f;
            ScaleY   = 1.0f;
        }
    };

    struct Sprite
    {
        Renderer2D::ResourceHandle ResourceHandle;

        wil::com_ptr<ID2D1Bitmap1> Bitmap;

        float Width{ 0.0f };

        float Height{ 0.0f };

        auto Reset() noexcept -> void
        {
            ResourceHandle = { .ID = Handle::ResourceID::Invalid, .Type = Handle::ResourceType::None, .Generation = Handle::Generation::Default };
            Bitmap.reset();
        }
    };

    struct Text
    {
        std::wstring Content;

        wil::com_ptr<IDWriteTextFormat> TextFormat;

        wil::com_ptr<IDWriteTextLayout> TextLayout;

        wil::com_ptr<ID2D1SolidColorBrush> Brush;

        bool IsDirty;

        auto Reset() noexcept -> void
        {
            Content.clear();
            TextFormat.reset();
            TextLayout.reset();
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
