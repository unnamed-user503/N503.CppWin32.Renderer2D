#include "Pch.hpp"
#include "TextSystem.hpp"

// 1. Project Headers
#include "../Device/Context.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System
{

    void TextSystem::Update(Registry& registry, Device::Context& context)
    {
        for (auto entity : registry.GetView<Transform, Text>())
        {
            auto& transform = registry.GetComponent<Transform>(entity);
            auto& text      = registry.GetComponent<Text>(entity);

            if (text.TextLayout)
            {
                context.DrawTextLayout({ transform.X, transform.Y }, text.TextLayout, text.Brush);
                // text.IsDirty = false;
            }
        }
    }

} // namespace N503::Renderer2D::System
