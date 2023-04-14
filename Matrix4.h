#pragma once
#include "Vector3.h"

class Matrix4
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
    namespace Matrix {
        // 単位行列
        Matrix4 Identity();

        // 拡大縮小行列
        Matrix4 Scale(const Vector3& s);

        // 回転行列
        Matrix4 RotateX(float angle);
        Matrix4 RotateY(float angle);
        Matrix4 RotateZ(float angle);

        // 平行移動行列
        Matrix4 Translate(const Vector3& t);

        // 転置行列
        Matrix4 Transpose(const Matrix4& m);

        // 座標変換
        Vector3 Transform(const Vector3& v, const Matrix4& m);

        // ビュー行列変換
        Matrix4 ViewLookToLH(const Vector3& eyePosition, const Vector3& eyeDirection, const Vector3& upDirection);
        Matrix4 ViewLookAtLH(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& upDirection);
    }
}
