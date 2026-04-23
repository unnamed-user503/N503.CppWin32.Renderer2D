#include "Pch.hpp"
#include "Registry.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Texture
{

    auto Registry::GetOrCreateBitmap(Renderer2D::AssetHandle handle) -> wil::com_ptr<ID2D1Bitmap1>
    {
        if (auto it = m_Cache.find(handle); it != m_Cache.end())
        {
            return it->second;
        }

        auto bitmap = m_Factory.CreateTextureFromHandle(handle);

        if (!bitmap)
        {
            return nullptr;
        }

        m_Cache[handle] = std::move(bitmap);
        return m_Cache[handle];
    }

    auto Registry::Clear() -> void
    {
        ++m_CurrentGeneration;
        m_Cache.clear();
    }

} // namespace N503::Renderer2D::Texture
