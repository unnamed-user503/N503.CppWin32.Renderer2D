#include "Pch.hpp"
#include "Registry.hpp"

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::System
{

    Registry::Registry() : m_World(std::make_unique<World>())
    {
        for (std::uint64_t entityId = 0; entityId < MaxEntities; ++entityId)
        {
            m_AvailableEntityIds.push_back(static_cast<Entity>(MaxEntities - entityId));
        }
    }

    auto Registry::CreateEntity() -> Entity
    {
        if (m_AvailableEntityIds.empty())
        {
            throw std::runtime_error("Maximum number of entities reached.");
        }

        // 利用可能なエンティティIDを取得
        auto entityID = m_AvailableEntityIds.back();
        m_AvailableEntityIds.pop_back();

        // エンティティを有効化
        auto index = ToArrayIndex(entityID);
        m_World->AliveBits.set(index);
        m_World->ComponentMasks[index].reset();

        // コンポーネントを初期化
        m_World->Transforms[index].Reset();
        m_World->Sprites[index].Reset();
        m_World->Texts[index].Reset();
        m_World->Colors[index].Reset();

        return entityID;
    }

    auto Registry::DestroyEntity(Entity entity) -> void
    {
        const auto index = ToArrayIndex(entity);

        if (ToArrayIndex(entity) >= MaxEntities)
        {
            throw std::runtime_error("Invalid entity ID.");
        }

        // エンティティを無効化
        m_World->AliveBits.reset(index);
        m_World->ComponentMasks[index].reset();

        // コンポーネントをリセット
        m_World->Transforms[index].Reset();
        m_World->Sprites[index].Reset();
        m_World->Texts[index].Reset();
        m_World->Colors[index].Reset();

        // エンティティIDを再利用可能なリストに戻す
        m_AvailableEntityIds.push_back(entity);
    }

} // namespace N503::Renderer2D::System
