#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Abi/Api.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <Windows.h>
#include <memory>

namespace N503::Renderer2D
{

    /// @brief
    class N503_API Bitmap final
    {
    public:
        /// @brief
        /// @param path
        Bitmap(const std::string_view path);

        /// @brief
        ~Bitmap();

        /// @brief
        /// @param
        Bitmap(const Bitmap&) = delete;
        /// @brief
        /// @param
        /// @return
        auto operator=(const Bitmap&) -> Bitmap& = delete;
        /// @brief
        /// @param
        Bitmap(Bitmap&&) noexcept;

        /// @brief
        /// @param
        /// @return
        auto operator=(Bitmap&&) noexcept -> Bitmap&;

    public:
    public:
        /// @brief 実装の詳細を隠蔽するための不透明な構造体。
        struct Entity;

#ifdef N503_DLL_EXPORTS
        /// @brief 内部の実装実体（Entity）への参照を取得します。
        /// @note
        /// このメソッドはライブラリ内部（DLL境界の内側）でのみ使用されます。
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
        /// @brief
        std::unique_ptr<Entity> m_Entity;
#pragma warning(pop)
    };

} // namespace N503::Renderer2D
