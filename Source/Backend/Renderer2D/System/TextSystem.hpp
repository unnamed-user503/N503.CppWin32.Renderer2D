#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 6. C++ Standard Libraries
#include <unordered_map>
#include <vector>

namespace N503::Renderer2D::System
{
    class Registry;
}

namespace N503::Renderer2D::System
{
    class TextSystem
    {
    public:
        TextSystem()  = default;

        ~TextSystem() = default;

        TextSystem(const TextSystem&) = delete;

        auto operator=(const TextSystem&) -> TextSystem& = delete;

        auto Update(Registry& registry) -> void;

    private:
        std::unordered_map<Entity, std::vector<Entity>> m_GlyphEntities;
    };

} // namespace N503::Renderer2D::System
