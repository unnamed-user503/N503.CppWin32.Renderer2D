#include "Pch.hpp"
#include "CommandList.hpp"

// 1. Project Headers
#include "Command.hpp"
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <type_traits>
#include <utility>
#include <variant>

namespace N503::Renderer2D::Device
{

    CommandList::CommandList()
    {
        Clear();
    }

    auto CommandList::Add(Command&& command) -> CommandHandle
    {
        // 空きスロットがない場合は無効なハンドルを返します。
        if (m_AvailableHandles.empty())
        {
            return { .ID = Handle::CommandID::Invalid };
        }

        // 利用可能なハンドルのリストから末尾を取得します。
        CommandHandle handle = m_AvailableHandles.back();
        m_AvailableHandles.pop_back();

        const auto index = static_cast<std::uint64_t>(handle.ID);

        m_Entries[index] = Entry{
            .Handle  = handle,
            .Command = std::move(command),
        };

        m_ActiveCount++;

        return handle;
    }

    auto CommandList::Remove(const CommandHandle handle) -> bool
    {
        const auto index = static_cast<std::uint64_t>(handle.ID);

        if (index >= MaxEntries)
        {
            return false;
        }

        // 世代チェック: すでに削除されているか、別のコマンドに再利用されている場合は無視します。
        if (m_Entries[index].Handle.Generation != handle.Generation)
        {
            return false;
        }

        // コマンドを破棄（std::monostate へ戻す）
        m_Entries[index].Command = std::monostate{};

        // 次の世代のハンドルを作成してフリーリストに戻します。
        CommandHandle nextHandle = handle;
        nextHandle.Generation    = static_cast<Handle::Generation>(static_cast<std::uint64_t>(handle.Generation) + 1);

        m_AvailableHandles.push_back(nextHandle);
        m_ActiveCount--;

        return true;
    }

    auto CommandList::Execute(Context& context) -> bool
    {
        if (m_ActiveCount == 0)
        {
            return false;
        }

        for (auto& entry : m_Entries)
        {
            // std::monostate (index 0) ではない有効なコマンドのみ実行します。
            if (entry.Command.index() != 0)
            {
                std::visit(
                    [&context](auto& command)
                    {
                        // std::monostate の場合は何もしない（型安全のためのコンパイル時分岐）
                        if constexpr (!std::is_same_v<std::decay_t<decltype(command)>, std::monostate>)
                        {
                            command.Execute(context);
                        }
                    },
                    entry.Command
                );
            }
        }

        return true;
    }

    auto CommandList::Clear() -> void
    {
        m_ActiveCount = 0;
        m_Entries.fill({ .Command = std::monostate{} });
        m_AvailableHandles.clear();

        // 0番から順に初期ハンドルをスタックに積みます（逆順に積むことで0番から使われるようにします）
        for (std::int64_t i = MaxEntries - 1; i >= 0; --i)
        {
            m_AvailableHandles.push_back({
                .ID         = static_cast<Handle::CommandID>(i),
                .Type       = Handle::CommandType::Draw,
                .Generation = Handle::Generation::Default,
            });
        }
    }

} // namespace N503::Renderer2D::Device
