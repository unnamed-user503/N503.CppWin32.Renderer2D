#include "Pch.hpp"
#include "SetContent.hpp"

// 1. Project Headers
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

    auto SetContent::operator()(Context& context) const -> void
    {
        if (!context.Registry.HasComponent<System::Component::Text>(ID))
        {
            return;
        }

        auto& text   = context.Registry.GetComponent<System::Component::Text>(ID);
        text.Content = TranscodeUtf8ToWide(Content);
        text.IsDirty = true;
    }

} // namespace N503::Renderer2D::Message::Packets
