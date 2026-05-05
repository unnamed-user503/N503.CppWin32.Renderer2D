#include "Pch.hpp"
#include "TypewriterSystem.hpp"

// 1. Project Headers
#include "Component/Text.hpp"
#include "Component/Typewriter.hpp"
#include "Entity.hpp"
#include "Registry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>

// Definition
namespace N503::Renderer2D::System
{

    TypewriterSystem::TypewriterSystem()
    {
    }

    auto TypewriterSystem::Update(Registry& registry, float deltaTime) -> void
    {
        for (auto entity : registry.GetView<Component::Text, Component::Typewriter>()) 
        {
            auto& text = registry.GetComponent<Component::Text>(entity);
            auto& typewriter   = registry.GetComponent<Component::Typewriter>(entity);

            if (!typewriter.IsDirty) continue;

            typewriter.Elapsed += deltaTime;
            float interval = 1.0f / typewriter.Speed;

            if (typewriter.Elapsed >= interval) 
            {
                // 蓄積時間から進めるべき文字数を計算
                int32_t advance = static_cast<int32_t>(typewriter.Elapsed / interval);
                text.VisibleCount += advance;
                typewriter.Elapsed -= advance * interval;

                // 文字数上限に達したら停止
                if (text.VisibleCount >= static_cast<int32_t>(text.Content.size())) 
                {
                    text.VisibleCount = static_cast<int32_t>(text.Content.size());
                    typewriter.IsDirty = false; 
                }
            }
        }
    }
}
