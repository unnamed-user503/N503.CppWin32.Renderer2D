#pragma once

// 1. Project Headers
#include "Batch/LineBatch.hpp"
#include "Batch/RectangleBatch.hpp"
#include "Batch/SpriteBatch.hpp"
#include "Batch/TextBatch.hpp"
#include "Device/Context.hpp"
#include "Texture/Registry.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>

// 6. C++ Standard Libraries
#include <array>
#include <unordered_map>
#include <variant>

namespace N503::Renderer2D
{

    class Processor final
    {
        static constexpr auto MaxBatchSize = 1000;

        using BatchVariant = std::variant<Batch::SpriteBatch /*, Batch::TextBatch, Batch::RectangleBatch, Batch::LineBatch*/>;

    public:
        auto AddBatch(const BatchVariant& batch) -> void;

        auto Process(Device::Context& context) -> bool;

    private:
        std::array<BatchVariant, MaxBatchSize> m_BatchArray;

        std::size_t m_CurrentBatchCount{ 0 };
    };

} // namespace N503::Renderer2D
