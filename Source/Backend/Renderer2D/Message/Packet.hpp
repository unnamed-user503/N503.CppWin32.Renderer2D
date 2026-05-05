#pragma once

// 1. Project Headers
#include "Packets/CreateSprite.hpp"
#include "Packets/CreateText.hpp"
#include "Packets/DestroyEntity.hpp"
#include "Packets/SetColor.hpp"
#include "Packets/SetContent.hpp"
#include "Packets/SetDepth.hpp"
#include "Packets/SetRenderGroup.hpp"
#include "Packets/SetTransform.hpp"
#include "Packets/SetVisible.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Message
{

    using namespace Packets;

    // clang-format off
    using Packet = std::variant<
        CreateSprite,
        CreateText,
        DestroyEntity,
        SetTransform,
        SetDepth,
        SetVisible,
        SetColor,
        SetRenderGroup,
        SetContent
    >;
    // clang-format on

} // namespace N503::Renderer2D::Message
