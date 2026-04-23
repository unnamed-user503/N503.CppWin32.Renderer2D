#include "Pch.hpp"
#include "Container.hpp"

// 1. Project Headers
#include "../Codec/WicImageDecoder.hpp"
#include "Asset.hpp"
#include "Metadata.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

namespace N503::Renderer2D::Resource
{

    Container::Container(std::size_t initialArenaSize) : m_Storage(initialArenaSize)
    {
        Reset();
    }

    Container::~Container()
    {
    }
    auto Container::Store(Renderer2D::Type type, std::string_view path) -> Renderer2D::AssetHandle
    {
        // 1. ハンドルの空き確認と払い出し (Audio と共通の管理ロジック)
        if (m_AvailableHandles.empty())
        {
            return { Renderer2D::Handle::ResourceID::InvalidValue };
        }

        const auto handle = m_AvailableHandles.back();
        const auto index  = static_cast<std::size_t>(handle.ResourceID);

        // 2. Arena から Asset 構造体自体のメモリを確保
        auto* assetMemory = m_Storage.AllocateBytes(sizeof(Resource::Asset), alignof(Resource::Asset));
        auto* asset       = new (assetMemory) Resource::Asset();

        // 3. 基本情報のセット
        asset->Handle        = handle;
        asset->Metadata.Path = std::string(path);
        // asset->Metadata.Type = type; // Metadata に Type を持たせる場合

        // 4. Type に応じたデコードとメモリ切り出し
        if (type == Renderer2D::Type::Bitmap)
        {
            // デコーダーのインスタンス化
            Codec::WicImageDecoder decoder(path);

            // アロケータを渡して Arena にピクセルを直接展開 (Audio::MediaFoundationDecoder と同じフロー)
            asset->Pixels = decoder.Decode(
                [this](std::size_t size)
                {
                    // 16バイトアライメントで Arena から切り出し
                    auto* ptr = m_Storage.AllocateBytes(size, 16);
                    return std::span<std::byte>(static_cast<std::byte*>(ptr), size);
                }
            );
        }
        else if (type == Renderer2D::Type::Movie)
        {
            // 将来的なストリーミング実装用の分岐
        }

        // 5. スロットへの登録とハンドル管理の更新
        m_AssetSlots[index] = asset;
        m_AvailableHandles.pop_back();

        return handle;
    }

    auto Container::GetAsset(Renderer2D::AssetHandle handle) const noexcept -> const Resource::Asset*
    {
        auto index = static_cast<std::size_t>(handle.ResourceID);

        if (index == 0 || index > MaxAssets)
        {
            return nullptr;
        }

        return m_AssetSlots[index];
    }

    auto Container::Remove(Renderer2D::AssetHandle handle) -> void
    {
        auto index = static_cast<std::size_t>(handle.ResourceID);

        if (index == 0 || index > MaxAssets)
        {
            return;
        }

        if (m_AssetSlots[index])
        {
            m_AssetSlots[index] = nullptr;
            m_AvailableHandles.push_back(handle);
        }
    }

    auto Container::Reset() -> void
    {
        m_Storage.Reset();
        m_AssetSlots.fill(nullptr);

        m_AvailableHandles.clear();
        m_AvailableHandles.reserve(MaxAssets);

        auto resourceIds = std::views::iota(0ULL, static_cast<unsigned long long>(MaxAssets));

        // clang-format off
        auto handleView = resourceIds | std::views::transform([](auto i)
        {
            return Renderer2D::AssetHandle{ .ResourceID = static_cast<Renderer2D::Handle::ResourceID>(i) };
        });
        // clang-format on

        std::ranges::copy(handleView, std::back_inserter(m_AvailableHandles));
    }

} // namespace N503::Renderer2D::Resource
