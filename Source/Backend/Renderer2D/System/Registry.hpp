#pragma once

// 1. Project Headers
#include "Component/Color.hpp"
#include "Component/Sprite.hpp"
#include "Component/Text.hpp"
#include "Component/Transform.hpp"
#include "Component/Typewriter.hpp"
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

    // -----------------------------------
    // ComponentTraits (single maintenance point)
    // -----------------------------------
    template <typename T> struct ComponentTraits;

    template <> struct ComponentTraits<Component::Transform>
    {
        static constexpr ComponentType Type = ComponentType::Transform;
        static auto& Get(World& world, std::size_t index)
        {
            return world.Transforms[index];
        }
    };

    template <> struct ComponentTraits<Component::Sprite>
    {
        static constexpr ComponentType Type = ComponentType::Sprite;
        static auto& Get(World& world, std::size_t index)
        {
            return world.Sprites[index];
        }
    };

    template <> struct ComponentTraits<Component::Text>
    {
        static constexpr ComponentType Type = ComponentType::Text;
        static auto& Get(World& world, std::size_t index)
        {
            return world.Texts[index];
        }
    };

    template <> struct ComponentTraits<Component::Color>
    {
        static constexpr ComponentType Type = ComponentType::Color;
        static auto& Get(World& world, std::size_t index)
        {
            return world.Colors[index];
        }
    };

    template <> struct ComponentTraits<Component::Typewriter>
    {
        static constexpr ComponentType Type = ComponentType::Typewriter;
        static auto& Get(World& world, std::size_t index)
        {
            return world.Typewriters[index];
        }
    };

    // -----------------------------------
    // View
    // -----------------------------------
    template <typename... Components> class View
    {
        World& m_World;

        std::bitset<MaxComponents> m_Mask;

    public:
        View(World& world) : m_World(world)
        {
            ((m_Mask.set(static_cast<std::size_t>(GetComponentType<Components>()))), ...);
        }

        struct Iterator
        {
            World& World;
            std::bitset<MaxComponents> Mask;
            std::size_t Index;

            auto operator*() const -> Entity
            {
                return static_cast<Entity>(Index);
            }

            auto operator++() -> Iterator&
            {
                do
                {
                    Index++;
                } while (Index < MaxEntities && (!World.AliveBits.test(Index) || (World.ComponentMasks[Index] & Mask) != Mask));
                return *this;
            }

            auto operator!=(const Iterator& other) const -> bool
            {
                return Index != other.Index;
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
        template <typename T> static constexpr auto GetComponentType() -> ComponentType
        {
            return ComponentTraits<T>::Type;
        }
    };

    // -----------------------------------
    // Registry
    // -----------------------------------
    class Registry
    {
    public:
        Registry();

        auto CreateEntity() -> Entity;

        auto DestroyEntity(Entity entity) -> void;

        auto GetAvailableEntityCount() const -> std::size_t
        {
            return m_AvailableEntityIds.size();
        }

        template <typename T> auto AddComponent(Entity entity, T&& component) -> T&
        {
            const auto index = ToArrayIndex(entity);

            auto& entry = ComponentTraits<T>::Get(*m_World, index);
            entry       = std::forward<T>(component);

            m_World->ComponentMasks[index].set(static_cast<size_t>(ComponentTraits<T>::Type));
            return entry;
        }

        template <typename T> auto GetComponent(Entity entity) -> T&
        {
            return ComponentTraits<T>::Get(*m_World, ToArrayIndex(entity));
        }

        template <typename T> bool HasComponent(Entity entity) const
        {
            return m_World->ComponentMasks[ToArrayIndex(entity)].test(static_cast<size_t>(ComponentTraits<T>::Type));
        }

        template <typename T> void RemoveComponent(Entity entity)
        {
            m_World->ComponentMasks[ToArrayIndex(entity)].reset(static_cast<size_t>(ComponentTraits<T>::Type));
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
