#pragma once

// 1. Project Headers
#include "../Pixels/Buffer.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Arena.hpp>
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace N503::Renderer2D::Resource
{

    class Container
    {
        static constexpr auto MaxEntries = 1024;

    public:
        Container();

        auto Add(const std::string_view path) -> ResourceHandle;

        auto Remove(const ResourceHandle handle) -> bool;

        auto Get(const ResourceHandle handle) const -> const Entry*;

        auto Get(const std::string_view path) const -> const Entry*;

        auto GetHandle(const std::string_view path) const -> ResourceHandle;

        auto Clear() -> void;

    private:
        N503::Memory::Storage::Arena m_Storage{ MaxEntries * 32768 };

        std::array<Entry, MaxEntries> m_Entries;

        std::vector<ResourceHandle> m_AvailableHandles;

        std::unordered_map<std::string_view, ResourceHandle, std::hash<std::string_view>, std::equal_to<>> m_Indexes;
    };

} // namespace N503::Renderer2D::Resource
