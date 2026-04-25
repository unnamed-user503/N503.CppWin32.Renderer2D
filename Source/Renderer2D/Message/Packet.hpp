#pragma once

// 1. Project Headers
#include "Packets/AddCommandList.hpp"
#include "Packets/CreateBitmap.hpp"
#include "Packets/DeleteBitmap.hpp"
#include "Packets/UpdateBitmapOpacity.hpp"
#include "Packets/UpdateBitmapPoint.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Renderer2D::Message
{

    using namespace Packets;

    using Packet = std::variant<CreateBitmap, DeleteBitmap, UpdateBitmapPoint, UpdateBitmapOpacity, AddCommandList>;

} // namespace N503::Renderer2D::Message
