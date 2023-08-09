#pragma once
#include "Vector3.h"

struct Matrix4 final
{
public:
    // 関数
    Matrix4(void) = default;
    Matrix4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);

    // 変数
    float m[4][4]{};
};

// 代入演算子オーバーロード
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// 2項演算子オーバーロード
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
const Vector3 operator*(const Vector3& v, const Matrix4& m);

namespace Math {
    namespace Mat4 {
        // 単位行列
        Matrix4 Identity();

        // 拡大縮小行列
        Matrix4 Scale(const Vector3& s);

        // 回転行列
        Matrix4 RotationX(float angle);
        Matrix4 RotationY(float angle);
        Matrix4 RotationZ(float angle);

        // 平行移動行列
        Matrix4 Translation(const Vector3& t);
        Matrix4 Translate(const Matrix4& matWorld, const Vector3& t); // 直接書き換えで高速化

        // 転置行列
        Matrix4 Transpose(const Matrix4& m);

        // 逆行列
        Matrix4 Inverse(const Matrix4& m);

        // 座標変換
        Vector3 Transform(const Vector3& v, const Matrix4& m);

        Matrix4 RotMatFromAxes3(const Vector3& forward, const Vector3& right, const Vector3& up);

        // ビュー行列
        Matrix4 ViewLookToLH(const Vector3& eyePosition, const Vector3& eyeDirection, const Vector3& upDirection);
        Matrix4 ViewLookAtLH(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& upDirection);

        // 射影行列
        Matrix4 ProjectionOrthoGraphicLH(float window_width, float window_height);
        Matrix4 ProjectionOrthoGraphicLH(float window_width, float window_height, float nearZ, float farZ);
        Matrix4 ProjectionOrthoGraphicLH(float leftEdge, float rightEdge, float bottomEdge, float topEdge, float nearZ, float farZ);

        Matrix4 ProjectionPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ);
        Matrix4 ProjectionPerspectiveFovLH(float fovY, float window_width, float window_height, float nearZ, float farZ);
    }
}
