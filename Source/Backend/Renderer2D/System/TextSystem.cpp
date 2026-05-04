#include "Pch.hpp"
#include "TextSystem.hpp"

// 1. Project Headers
#include "../Canvas/Cache.hpp"
#include "../Canvas/Device.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System
{

    void TextSystem::Update(Registry& registry, Canvas::Device& device)
    {
        using namespace Component;

        for (auto entity : registry.GetView<Transform, Text>())
        {
            auto& transform = registry.GetComponent<Transform>(entity);
            auto& text      = registry.GetComponent<Text>(entity);

            if (text.Content.empty())
            {
                continue;
            }

            if (!text.TextFormat)
            {
                text.TextFormat = device.GetOrCreateTextFormat(text.FontName, text.FontSize);

                if (!text.TextFormat)
                {
                    continue;
                }
            }

            if (!text.TextLayout)
            {
                text.TextLayout = device.GetOrCreateTextLayout(text.Content, text.TextFormat);
            }

            if (!text.Brush)
            {
                text.Brush = device.GetOrCreateBrush({ text.Color.r, text.Color.g, text.Color.b, text.Color.a });
            }
            /*
            if (text.TextLayout && text.Brush)
            {
                device.DrawTextLayout({ transform.Position.X, transform.Position.Y }, text.TextLayout, text.Brush);
            }
            */
        }
    }

} // namespace N503::Renderer2D::System
