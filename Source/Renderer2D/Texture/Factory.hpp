#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>

// 6. C++ Standard Libraries
#include <memory>
#include <string>

namespace N503::Renderer2D::Texture
{

    class Factory final
    {
    public:
        auto CreateTextureFromHandle(Renderer2D::AssetHandle handle) -> wil::com_ptr<ID2D1Bitmap1>;
    };

}
