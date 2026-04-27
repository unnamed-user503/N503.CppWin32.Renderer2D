#include "Pch.hpp"
#include "Container.hpp"

// 1. Project Headers
#include "../Codec/WicImageDecoder.hpp"
#include "../Pixels/Buffer.hpp"
#include "../Engine.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>
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
#include <format>

namespace N503::Renderer2D::Resource
{

    Container::Container()
    {
        Clear();
    }

    auto Container::Add(const std::string_view path) -> ResourceHandle
    {
        // すでに同じパスのリソースが存在する場合は、そのハンドルを返します。
        if (auto it = m_Indexes.find(path); it != m_Indexes.end())
        {
            return it->second;
        }

        // 利用可能なハンドルがない場合は、無効なハンドルを返します。
        if (m_AvailableHandles.empty())
        {
            return { .ID = Handle::ResourceID::Invalid };
        }

        // 利用可能なハンドルのリストからハンドルを取得します。
        ResourceHandle handle = m_AvailableHandles.back();

        // ハンドルのIDをインデックスとして使用します。これにより、O(1)でエントリにアクセスできます。
        const auto index = static_cast<std::uint64_t>(handle.ID);

        // 画像をデコードして、確保したメモリにピクセルデータを書き込みます。
        Codec::WicImageDecoder decoder(path);

        Pixels::Buffer pixels;
        pixels.Size = static_cast<std::size_t>(decoder.GetHeight()) * static_cast<std::size_t>(decoder.GetPitch());

        // 画像データのバイトサイズに基づいて、アリメントを16バイトとした生のバイト列としてメモリを確保します。
        void* address = m_Storage.AllocateBytes(pixels.Size, 16);

        if (address == nullptr)
        {
            return {}; // メモリ確保に失敗した場合は、無効なハンドルを返します。
        }

        pixels = decoder.Decode(
            [&](std::size_t size) -> std::span<std::byte>
            {
                if (size > pixels.Size)
                {
                    return {};
                }
                return std::span<std::byte>(static_cast<std::byte*>(address), size);
            }
        );

        // エントリを作成して保存します。
        m_Entries[index] = Entry{
            .Handle   = handle,
            .Pixels   = pixels,
            .Metadata = { .Path = std::string(path) },
        };

        // パスとハンドルの対応を保存します。
        m_Indexes[m_Entries[index].Metadata.Path] = handle;
        
#ifdef _DEBUG
        auto log = std::format("[Renderer2D] <Resource::Container>: Add to ResourceID={}", static_cast<std::uint64_t>(handle.ID));
        Engine::GetInstance().GetDiagnosticsSink().AddEntry(Diagnostics::Entry{ Diagnostics::Severity::Verbose, log });
#endif

        // 利用可能なハンドルのリストからハンドルを削除します。
        m_AvailableHandles.pop_back();

        return handle;
    }

    auto Container::Remove(const ResourceHandle handle) -> bool
    {
        // ハンドルのIDをインデックスとして使用します。
        const auto index = static_cast<std::uint64_t>(handle.ID);

        if (index >= MaxEntries)
        {
            return false;
        }

        // ハンドルの世代がエントリの世代と一致しない場合は、すでに無効であるか、あるいは世代が古いとみなします。
        if (m_Entries[index].Handle.Generation != handle.Generation)
        {
            return false; // すでに無効、あるいは世代が古い
        }

        // パスとハンドルの対応を削除します。
        m_Indexes.erase(m_Entries[index].Metadata.Path);

        // エントリを空にして、次の世代のハンドルを利用可能なハンドルのリストに追加します。
        ResourceHandle nextGenerationHandle = handle;
        nextGenerationHandle.Generation     = static_cast<Handle::Generation>(static_cast<std::uint64_t>(handle.Generation) + 1);

        // エントリを空にします。
        m_Entries[index] = {};

        // 次の世代のハンドルを利用可能なハンドルのリストに追加します。
        m_AvailableHandles.push_back(nextGenerationHandle);

        return true;
    }

    auto Container::Get(const ResourceHandle handle) const -> const Entry*
    {
        const auto index = static_cast<std::uint64_t>(handle.ID);

        if (index >= MaxEntries)
        {
            return nullptr;
        }

        if (m_Entries[index].Handle.Generation != handle.Generation)
        {
            return nullptr; // 無効、あるいは世代が古い
        }

        return &m_Entries[index];
    }

    auto Container::Get(const std::string_view path) const -> const Entry*
    {
        if (auto it = m_Indexes.find(path); it != m_Indexes.end())
        {
            return Get(it->second);
        }

        return nullptr;
    }

    auto Container::GetHandle(const std::string_view path) const -> ResourceHandle
    {
        if (auto it = m_Indexes.find(path); it != m_Indexes.end())
        {
            return it->second;
        }

        return { .ID = Handle::ResourceID::Invalid };
    }

    auto Container::Clear() -> void
    {
        // すべてのエントリをクリアし、利用可能なハンドルのリストを初期化します。
        m_Indexes.clear();
        m_Entries.fill({});
        m_AvailableHandles.clear();

        // Arenaの状態をリセットします。これにより、すべての確保されたメモリが解放され、次の確保は最初のブロックから始まります。
        m_Storage.Reset();

        // 利用可能なハンドルのリストを初期化します。これにより、最初のMaxEntries個のリソースIDが利用可能になります。
        for (std::uint64_t i = 0; i < MaxEntries; ++i)
        {
            m_AvailableHandles.push_back({ .ID = static_cast<Handle::ResourceID>(i), .Generation = Handle::Generation::Default });
        }
    }

} // namespace N503::Renderer2D::Resource
