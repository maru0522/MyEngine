#include "MathUtil.h"
#include "Matrix4.h"
#include "Vector3.h"
#include <cmath> // sin cos

// 単位行列を求める
Matrix4 Math::Matrix::Identity()
{
    static const Matrix4 result
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return result;
}

// 拡大縮小行列を求める
Matrix4 Math::Matrix::Scale(const Vector3& s)
{
    Matrix4 result
    {
         s.x, 0.0f, 0.0f, 0.0f,
        0.0f,  s.y, 0.0f, 0.0f,
        0.0f, 0.0f,  s.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return result;
}

// x 軸まわりの回転行列を求める
Matrix4 Math::Matrix::RotationX(float angle)
{
    float sin = std::sin(angle);
    float cos = std::cos(angle);

    Matrix4 result
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  cos,  sin, 0.0f,
        0.0f, -sin,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return result;
}

// y 軸まわりの回転行列を求める
Matrix4 Math::Matrix::RotationY(float angle)
{
    float sin = std::sin(angle);
    float cos = std::cos(angle);

    Matrix4 result
    {
         cos, 0.0f, -sin, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
         sin, 0.0f,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return result;
}

// z 軸まわりの回転行列を求める
Matrix4 Math::Matrix::RotationZ(float angle)
{
    float sin = std::sin(angle);
    float cos = std::cos(angle);

    Matrix4 result
    {
         cos,  sin, 0.0f, 0.0f,
        -sin,  cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    return result;
}

// 平行移動行列を求める
Matrix4 Math::Matrix::Translation(const Vector3& t)
{
    Matrix4 result
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
         t.x,  t.y,  t.z, 1.0f,
    };
    return result;
}

Matrix4 Math::Matrix::Translate(const Matrix4& matWorld, const Vector3& t)
{
    return Matrix4{
        matWorld.m[0][0], matWorld.m[0][1], matWorld.m[0][2], matWorld.m[0][3],
        matWorld.m[1][0], matWorld.m[1][1], matWorld.m[1][2], matWorld.m[1][3],
        matWorld.m[2][0], matWorld.m[2][1], matWorld.m[2][2], matWorld.m[2][3],
                     t.x,              t.y,              t.z, matWorld.m[3][3]
    };
}

Matrix4 Math::Matrix::Transpose(const Matrix4& m)
{
    Matrix4 result
    {
        m.m[0][0],m.m[1][0],m.m[2][0],m.m[3][0],
        m.m[0][1],m.m[1][1],m.m[2][1],m.m[3][1],
        m.m[0][2],m.m[1][2],m.m[2][2],m.m[3][2],
        m.m[0][3],m.m[1][3],m.m[2][3],m.m[3][3]
    };
    return result;
}

// 座標変換（ベクトル行列の掛け算）を行うTransform 関数を作成する。　（透視変換にも対応している）
Vector3 Math::Matrix::Transform(const Vector3& v, const Matrix4& m)
{
    float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

    Vector3 result
    {
        (v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0]) / w,
        (v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1]) / w,
        (v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]) / w,
    };
    return result;
}

Matrix4 Math::Matrix::ViewLookToLH(const Vector3& eyePosition, const Vector3& eyeDirection, const Vector3& upDirection)
{
    auto axisZ{ eyeDirection.normalize() };
    auto axisX{ Math::Vector::Normalize(upDirection.cross(eyeDirection)) };
    auto axisY{ Math::Vector::Normalize(axisZ.cross(axisX)) };

    return Matrix4{
                        axisX.x,                 axisY.x,                 axisZ.x, 0,
                        axisX.y,                 axisY.y,                 axisZ.y, 0,
                        axisX.z,                 axisY.z,                 axisZ.z, 0,
        -axisX.dot(eyePosition), -axisY.dot(eyePosition), -axisZ.dot(eyePosition), 1
    };
}

Matrix4 Math::Matrix::ViewLookAtLH(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& upDirection)
{
    return ViewLookToLH(eyePosition, targetPosition - eyePosition, upDirection);
}

Matrix4 Math::Matrix::ProjectionOrthoGraphicLH(float window_width, float window_height)
{
    return ProjectionOrthoGraphicLH(0, window_width, 0, window_height, 0.f, 1.f);
}

Matrix4 Math::Matrix::ProjectionOrthoGraphicLH(float window_width, float window_height, float nearZ, float farZ)
{
    return ProjectionOrthoGraphicLH(0, window_width, 0, window_height, nearZ, farZ);
}

Matrix4 Math::Matrix::ProjectionOrthoGraphicLH(float leftEdge, float rightEdge, float bottomEdge, float topEdge, float nearZ, float farZ)
{
    auto M00{ 2.f / (rightEdge - leftEdge) };
    auto M11{ -2.f / (topEdge - bottomEdge) };
    auto M22{ 2.f / (farZ - nearZ) };

    return Matrix4{
         M00, 0.f, 0.f, 0.f,
         0.f, M11, 0.f, 0.f,
         0.f, 0.f, M22, 0.f,
        -1.f, 1.f, 0.f, 1.f
    };
}

Matrix4 Math::Matrix::ProjectionPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ)
{
    auto scaleY = Math::Function::Cotangent(fovY / 2);
    auto scaleX = scaleY / aspect;

    return Matrix4{
        scaleX,    0.f,                            0.f, 0.f,
           0.f, scaleY,                            0.f, 0.f,
           0.f,    0.f,          farZ / (farZ - nearZ), 1.f,
           0.f,    0.f, -nearZ * farZ / (farZ / nearZ), 0.f
    };
}

Matrix4 Math::Matrix::ProjectionPerspectiveFovLH(float fovY, float window_width, float window_height, float nearZ, float farZ)
{
    return ProjectionPerspectiveFovLH(fovY, window_width / window_height, nearZ, farZ);
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

// 代入演算子 *= のオーバロード関数（行列と行列の積)
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2)
{
    Matrix4 result{ };
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                result.m[i][j] += m1.m[i][k] * m2.m[k][j];
            }
        }
    }
    m1 = result;
    return m1;
}

// 2項演算子 * のオーバロード関数（行列と行列の積）
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
    Matrix4 result = m1;
    return result *= m2;
}

// 2項演算子 * の　オーバロード関数（ベクトルと行列の積）
const Vector3 operator*(const Vector3& v, const Matrix4& m)
{
    return Math::Matrix::Transform(v, m);
}

