#pragma once

// 1. Project Headers
#include "Factory.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>

// 6. C++ Standard Libraries
#include <memory>
#include <string>
#include <unordered_map>

namespace N503::Renderer2D::Texture
{

    class Registry final
    {
    public:
        auto GetOrCreateBitmap(Renderer2D::AssetHandle handle) -> wil::com_ptr<ID2D1Bitmap1>;

        auto Clear() -> void;

        [[nodiscard]]
        auto GetCurrentGeneration() const noexcept -> std::uint64_t
        {
            return m_CurrentGeneration;
        }

    private:
        Texture::Factory m_Factory;

        std::unordered_map<Renderer2D::AssetHandle, wil::com_ptr<ID2D1Bitmap1>> m_Cache;

        std::uint64_t m_CurrentGeneration{ 1 };
    };

} // namespace N503::Renderer2D::Texture
