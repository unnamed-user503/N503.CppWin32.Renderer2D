#pragma once

// 1. Project Headers
#include "Command.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <cstdint>
#include <vector>

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device
{
    class CommandList
    {
        static constexpr auto MaxEntries = 1024;

    public:
        CommandList();

        // コマンドを追加し、ハンドルを返します。
        auto Add(Command&& command) -> CommandHandle;

        // ハンドルを指定してコマンドを削除します。
        auto Remove(const CommandHandle handle) -> bool;

        // 現在登録されているすべてのコマンドを実行します。
        auto Execute(Context& context) -> bool;

        // すべてのコマンドをクリアし、初期状態に戻します。
        auto Clear() -> void;

        auto GetCount() const noexcept -> std::size_t
        {
            return m_ActiveCount;
        }

    private:
        struct Entry
        {
            CommandHandle Handle;

            Command Command;
        };

        std::array<Entry, MaxEntries> m_Entries{};

        std::vector<CommandHandle> m_AvailableHandles{};

        std::size_t m_ActiveCount{ 0 };
    };

} // namespace N503::Renderer2D::Device
