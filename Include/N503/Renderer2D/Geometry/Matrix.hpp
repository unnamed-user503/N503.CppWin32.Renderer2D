#pragma once

#include <cmath>

namespace N503::Renderer2D::Geometry
{
    struct Matrix3x2
    {
        // Direct2D互換のメンバ変数 (行優先: Row-major)
        float _11, _12; // 拡大縮小・回転
        float _21, _22; // 拡大縮小・回転
        float _31, _32; // 平行移動 (dx, dy)

        // 単位行列
        static constexpr Matrix3x2 Identity() {
            return { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
        }

        // 行列の乗算 (Direct2Dと同じ A * B の順序)
        // Aを適用した後にBを適用する「左から右」の合成
        Matrix3x2 operator*(const Matrix3x2& other) const {
            return {
                _11 * other._11 + _12 * other._21,
                _11 * other._12 + _12 * other._22,
                _21 * other._11 + _22 * other._21,
                _21 * other._12 + _22 * other._22,
                _31 * other._11 + _32 * other._21 + other._31,
                _31 * other._12 + _32 * other._22 + other._32
            };
        }

        // 基本的な静的ファクトリ
        static Matrix3x2 Translation(float x, float y)
        {
            return { 1.0f, 0.0f, 0.0f, 1.0f, x, y };
        }

        static Matrix3x2 Scale(float x, float y)
        {
            return { x, 0.0f, 0.0f, y, 0.0f, 0.0f };
        }

        static Matrix3x2 Rotation(float angleDegree)
        {
            // 度数法からラジアンへ変換
            float radian = angleDegree * (3.1415926535f / 180.0f);
            float sin = std::sin(radian);
            float cos = std::cos(radian);

            // Direct2D互換の回転行列レイアウト
            return {
                 cos, sin,
                -sin, cos,
                 0, 0
            };
        }
    };
}
