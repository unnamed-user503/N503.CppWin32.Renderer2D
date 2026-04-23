#include "Pch.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "CreateResourceCommand.hpp"

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <cstdint>
#include <string_view>

namespace N503::Renderer2D::Command::Packets
{

    auto CreateResourceCommand::operator()() const -> void
    {
        if (auto assetHandle = Renderer2D::Engine::Instance().GetResourceContainer().Store(Renderer2D::Type::Bitmap, Path))
        {
            auto asset = Renderer2D::Engine::Instance().GetResourceContainer().GetAsset(assetHandle);

            float x      = 0.0f;
            float y      = 0.0f;
            float width  = static_cast<float>(asset->Pixels.Width);
            float height = static_cast<float>(asset->Pixels.Height);

            auto batch = Batch::SpriteBatch{
                .Handle = assetHandle,
                .TargetRect = {
                    .left   = x,
                    .top    = y,
                    .right  = x + width,
                    .bottom = y + height,
                },
                .SourceRect = {
                    .left   = 0.0f,
                    .top    = 0.0f,
                    .right  = width,
                    .bottom = height,
                },
                .Opacity = 1.0f,
            };

            Renderer2D::Engine::Instance().GetBatchProcessor().AddBatch(batch);
        }
    }

} // namespace N503::Renderer2D::Command::Packets
