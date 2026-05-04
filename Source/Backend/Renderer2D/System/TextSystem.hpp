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

        // コピー禁止
        TextSystem(const TextSystem&)                    = delete;

        auto operator=(const TextSystem&) -> TextSystem& = delete;

        auto Update(Registry& registry) -> void;

    private:
        // テキストエンティティ → グリフ子エンティティ一覧
        std::unordered_map<Entity, std::vector<Entity>> m_GlyphEntities;

        auto DestroyGlyphs(Registry& registry, Entity textEntity) -> void;
        auto BuildGlyphs(Registry& registry, Entity textEntity) -> void;
    };

} // namespace N503::Renderer2D::System
