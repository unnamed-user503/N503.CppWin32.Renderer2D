#pragma once

// 1. Project Headers
#include "Asset.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Arena.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <string_view>
#include <vector>

namespace N503::Renderer2D::Resource
{

    class Container final
    {
    public:
        static constexpr std::size_t MaxAssets = 1024;

        explicit Container(std::size_t initialArenaSize = 1024 * 1024 * 16);

        ~Container();

        Container(const Container&) = delete;

        auto operator=(const Container&) -> Container& = delete;

        auto Store(Renderer2D::Type type, std::string_view path) -> Renderer2D::AssetHandle;

        auto GetAsset(Renderer2D::AssetHandle handle) const noexcept -> const Resource::Asset*;

        auto Remove(Renderer2D::AssetHandle handle) -> void;

        auto Reset() -> void;

    private:
        N503::Memory::Storage::Arena m_Storage;

        std::array<Resource::Asset*, MaxAssets + 1> m_AssetSlots{};

        std::vector<Renderer2D::AssetHandle> m_AvailableHandles;
    };

} // namespace N503::Renderer2D::Resource
