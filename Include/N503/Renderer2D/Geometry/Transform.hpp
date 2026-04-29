#pragma once

#include <N503/Renderer2D/Geometry/Point.hpp>
#include <N503/Renderer2D/Geometry/Matrix.hpp>

namespace N503::Renderer2D::Geometry
{

    struct Transform
    {
        Point3F Position;

        float Rotation{ 0.0f };

        Point2F Scale;

        Matrix3x2 ToMatrix() const
        {
            // S * R * T の順序で合成することで、
            // 「自身の中心で拡大し、回転し、指定位置へ移動する」挙動になる
            return Matrix3x2::Scale(Scale.X, Scale.Y) *
                   Matrix3x2::Rotation(Rotation) *
                   Matrix3x2::Translation(Position.X, Position.Y);
        }
    };

} // namespace N503::Renderer2D::Geometry
