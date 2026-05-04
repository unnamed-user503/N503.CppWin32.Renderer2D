#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <unordered_map>
#include <vector>

namespace N503::Renderer2D::Canvas
{
    class Session;
}

namespace N503::Renderer2D::System
{
    class Registry;
}

namespace N503::Renderer2D::System
{

    class RendererSystem
    {
        struct DrawCommand
        {
            ID2D1Bitmap1* Bitmap{ nullptr };
            D2D1_RECT_F DestinationRect{};
            D2D1_RECT_U SourceRect{};
            D2D1_COLOR_F Color{};
            D2D1_MATRIX_3X2_F Matrix{};
        };
        // 「AddSprites が期待する順序通りに並んでいること」だけを保証する
        static_assert(offsetof(DrawCommand, DestinationRect) < offsetof(DrawCommand, SourceRect));
        static_assert(offsetof(DrawCommand, SourceRect) < offsetof(DrawCommand, Color));
        static_assert(offsetof(DrawCommand, Color) < offsetof(DrawCommand, Matrix));

        struct IdentityHash
        {
            auto operator()(std::uint16_t value) const -> std::size_t
            {
                return static_cast<std::size_t>(value);
            }
        };

    public:
        auto Update(Registry& registry, Canvas::Session& session) -> void;

    private:
        std::unordered_map<Entity, D2D1_MATRIX_3X2_F, IdentityHash> m_TransformCache;
    };

} // namespace N503::Renderer2D::System
