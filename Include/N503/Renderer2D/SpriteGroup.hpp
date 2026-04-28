#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Abi/Api.hpp>

// 3. WIL (Windows Implementation Library)
#include <N503/Renderer2D/Types.hpp>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

namespace N503::Renderer2D
{

    class N503_API SpriteGroup final
    {
    public:
        explicit SpriteGroup(const std::string_view path, std::size_t count);

        ~SpriteGroup();

        SpriteGroup(const SpriteGroup&) = delete;

        auto operator=(const SpriteGroup&) -> SpriteGroup& = delete;

        SpriteGroup(SpriteGroup&&);

        auto operator=(SpriteGroup&&) -> SpriteGroup&;

    public:
        auto SetTransform(std::function<bool(const std::uint64_t index, Transform& transform)> delegate) -> void;

    public:
        struct Entity;

#ifdef N503_DLL_EXPORTS
        /// @brief 内部の実装実体（Entity）への参照を取得します。
        /// @note このメソッドはライブラリ内部（DLL境界の内側）でのみ使用されます。
        /// @return Entity を管理する unique_ptr への参照。
        [[nodiscard]]
        auto GetEntity() -> std::unique_ptr<Entity>&
        {
            return m_Entity;
        }
#endif

    private:
#pragma warning(push)
#pragma warning(disable : 4251) // DLL境界を越える際の unique_ptr に関する警告を抑制
        std::unique_ptr<Entity> m_Entity;
#pragma warning(pop)
    };

} // namespace N503::Renderer2D
