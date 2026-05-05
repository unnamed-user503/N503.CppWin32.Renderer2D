#pragma once

// 1. Project Headers
#include "Component/Color.hpp"
#include "Component/Depth.hpp"
#include "Component/Sprite.hpp"
#include "Component/Text.hpp"
#include "Component/Transform.hpp"
#include "Component/Typewriter.hpp"
#include "Component/Visible.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <bitset>
#include <cstdint>

namespace N503::Renderer2D::System
{

    static constexpr std::size_t MaxEntities = 32768;

    static constexpr std::size_t MaxComponents = 7;

    using Entity = std::uint16_t;

    [[nodiscard]]
    constexpr std::size_t ToArrayIndex(Entity entity) noexcept
    {
        return static_cast<std::size_t>(entity);
    }

    enum class ComponentType : std::size_t
    {
        Transform = 0,
        Visible,
        Depth,
        Color,
        Sprite,
        Text,
        Typewriter,
    };

    // require:
    // auto world = std::make_unique<N503::Renderer2D::System::World>();
    struct World
    {
        std::bitset<MaxEntities> AliveBits;

        std::array<std::bitset<MaxComponents>, MaxEntities> ComponentMasks;

        std::array<Component::Transform, MaxEntities> Transforms;

        std::array<Component::Visible, MaxEntities> Visibles;

        std::array<Component::Depth, MaxEntities> Depths;

        std::array<Component::Color, MaxEntities> Colors;

        std::array<Component::Sprite, MaxEntities> Sprites;

        std::array<Component::Text, MaxEntities> Texts;

        std::array<Component::Typewriter, MaxEntities> Typewriters;
    };

} // namespace N503::Renderer2D::System
