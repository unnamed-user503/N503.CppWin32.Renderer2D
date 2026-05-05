#include "Pch.hpp"
#include "CreateText.hpp"

// 1. Project Headers
#include "../../Canvas/Device.hpp"
#include "../../Message/Context.hpp"
#include "../../System/Entity.hpp"
#include "../../System/Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Message::Packets
{

    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    auto CreateText::operator()(Message::Context& context) const -> void
    {
        try
        {
            auto entity = context.Registry.CreateEntity();

            auto& transform      = context.Registry.AddComponent(entity, System::Component::Transform{});
            transform.Position.X = 0.0f;
            transform.Position.Y = 0.0f;
            transform.Rotation   = 0.0f;
            transform.Scale.X    = 0.0f;
            transform.Scale.Y    = 0.0f;

            auto& text    = context.Registry.AddComponent(entity, System::Component::Text{});
            text.Content  = TranscodeUtf8ToWide(Text);
            text.FontName = TranscodeUtf8ToWide(FontName);
            text.FontSize = FontSize;
            text.Atlas    = context.CanvasDevice.GetOrCreateFontAtlas(text.FontName, text.FontSize);
            text.VisibleCount = 0;
            text.IsDirty  = true;

            auto& color = context.Registry.AddComponent(entity, System::Component::Color{ 1.0f, 1.0f, 1.0f, 1.0f });

            auto& typewriter = context.Registry.AddComponent(entity, System::Component::Typewriter{ .Speed = 20.0f });
            typewriter.IsDirty = true;

            *Result = entity;
        }
        catch (...)
        {
        }
    }

} // namespace N503::Renderer2D::Message::Packets
