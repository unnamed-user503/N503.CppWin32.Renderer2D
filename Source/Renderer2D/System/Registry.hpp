#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>

namespace N503::Renderer2D::System
{

    template <typename... Components> class View
    {
        World& m_World;

        std::bitset<4> m_Mask;

    public:
        View(World& world) : m_World(world)
        {
            ((m_Mask.set(static_cast<std::size_t>(GetComponentType<Components>()))), ...);
        }

        struct Iterator
        {
            World& world;
            std::bitset<4> mask;
            std::size_t index;

            auto operator*() const -> Entity
            {
                return static_cast<Entity>(index);
            }

            auto operator++() -> Iterator&
            {
                do
                {
                    index++;
                } while (index < MaxEntities && (!world.AliveBits.test(index) || (world.ComponentMasks[index] & mask) != mask));
                return *this;
            }

            auto operator!=(const Iterator& other) const -> bool
            {
                return index != other.index;
            }
        };

        auto begin()
        {
            std::size_t first = 0;
            while (first < MaxEntities && (!m_World.AliveBits.test(first) || (m_World.ComponentMasks[first] & m_Mask) != m_Mask))
            {
                first++;
            }
            return Iterator{ m_World, m_Mask, first };
        }

        auto end()
        {
            return Iterator{ m_World, m_Mask, MaxEntities };
        }

    private:
        template <typename T> static constexpr ComponentType GetComponentType()
        {
            if constexpr (std::is_same_v<T, Transform>)
            {
                return ComponentType::Transform;
            }
            if constexpr (std::is_same_v<T, Sprite>)
            {
                return ComponentType::Sprite;
            }
            if constexpr (std::is_same_v<T, Text>)
            {
                return ComponentType::Text;
            }
        }
    };

    class Registry
    {
    public:
        Registry();

        auto CreateEntity() -> Entity;

        auto DestroyEntity(Entity entity) -> void;

        template <typename T> auto AddComponent(Entity entity, T&& component) -> std::decay_t<T>&
        {
            const auto index = ToArrayIndex(entity);

            if constexpr (std::is_same_v<std::decay_t<T>, Transform>)
            {
                m_World->Transforms[index] = std::forward<T>(component);
                m_World->ComponentMasks[index].set(static_cast<std::size_t>(ComponentType::Transform));
                return m_World->Transforms[index];
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, Sprite>)
            {
                m_World->Sprites[index] = std::forward<T>(component);
                m_World->ComponentMasks[index].set(static_cast<std::size_t>(ComponentType::Sprite));
                return m_World->Sprites[index];
            }
            else if constexpr (std::is_same_v<std::decay_t<T>, Text>)
            {
                m_World->Texts[index] = std::forward<T>(component);
                m_World->ComponentMasks[index].set(static_cast<std::size_t>(ComponentType::Text));
                return m_World->Texts[index];
            }
        }

        // コンポーネントへの参照を取得する
        template <typename T> [[nodiscard]] auto GetComponent(Entity entity) -> T&
        {
            const auto index = ToArrayIndex(entity);
            if constexpr (std::is_same_v<T, Transform>)
            {
                return m_World->Transforms[index];
            }
            if constexpr (std::is_same_v<T, Sprite>)
            {
                return m_World->Sprites[index];
            }
            if constexpr (std::is_same_v<T, Text>)
            {
                return m_World->Texts[index];
            }
        }

        // 特定のコンポーネントを持っているか確認
        template <typename T> [[nodiscard]] auto HasComponent(Entity entity) const -> bool
        {
            const auto index = ToArrayIndex(entity);
            if constexpr (std::is_same_v<T, Transform>)
            {
                return m_World->ComponentMasks[index].test(ComponentType::Transform);
            }
            if constexpr (std::is_same_v<T, Sprite>)
            {
                return m_World->ComponentMasks[index].test(ComponentType::Sprite);
            }
            if constexpr (std::is_same_v<T, Text>)
            {
                return m_World->ComponentMasks[index].test(ComponentType::Text);
            }
            return false;
        }

        template <typename T> auto RemoveComponent(Entity entity) -> void
        {
            const auto index = ToArrayIndex(entity);
            if constexpr (std::is_same_v<T, Transform>)
            {
                m_World->ComponentMasks[index].reset(ComponentType::Transform);
            }
            if constexpr (std::is_same_v<T, Sprite>)
            {
                m_World->ComponentMasks[index].reset(ComponentType::Sprite);
            }
            if constexpr (std::is_same_v<T, Text>)
            {
                m_World->ComponentMasks[index].reset(ComponentType::Text);
            }
        }

        template <typename... Components> [[nodiscard]] auto GetView() -> View<Components...>
        {
            return View<Components...>(*m_World);
        }

    private:
        std::vector<Entity> m_AvailableEntityIds;

        std::unique_ptr<Renderer2D::System::World> m_World;
    };

} // namespace N503::Renderer2D::System
