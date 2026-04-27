#include "Pch.hpp"
#include "CreateText.hpp"

// 1. Project Headers
#include "../../Device/Context.hpp"
#include "../../Message/Context.hpp"
#include "../../Resource/Container.hpp"
#include "../../System/Entity.hpp"
#include "../../System/Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    auto CreateText::operator()(Message::Context& context) const -> void
    {
        auto textFormat = context.DeviceContext.GetResourceCache().GetOrCreateTextFormat(FontName, FontSize);
        if (!textFormat)
        {
            return;
        }

        auto textLayout = context.DeviceContext.GetResourceCache().GetOrCreateTextLayout(Text, textFormat);
        if (!textLayout)
        {
            return;
        }

        auto brush = context.DeviceContext.GetResourceCache().GetOrCreateBrush(Color);
        if (!textLayout)
        {
            return;
        }

        try
        {
            auto entity = context.Registry.CreateEntity();

            auto& transform    = context.Registry.AddComponent(entity, System::Transform{});
            transform.X        = 0.0f;
            transform.Y        = 0.0f;
            transform.ScaleX   = 1.0f;
            transform.ScaleY   = 1.0f;
            transform.Rotation = 0.0f;

            auto& text      = context.Registry.AddComponent(entity, System::Text{});
            text.Brush      = brush;
            text.TextFormat = textFormat;
            text.TextLayout = textLayout;
            text.IsDirty    = true;
            text.Content    = L"Hello";
        }
        catch (...)
        {
        }
    }

} // namespace N503::Renderer2D::Message::Packets
