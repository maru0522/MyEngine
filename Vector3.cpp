#include "Vector3.h"
#include <cmath>

// 引数なしコンストラクタ
// 零ベクトルに初期化する
Vector3::Vector3(void)
    : x(0), y(0), z(0)
{
}

// 引数3つのコンストラクタ
Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

Vector3::Vector3(DirectX::XMFLOAT3 xmf3)
    : x(xmf3.x), y(xmf3.y), z(xmf3.z)
{
}

// ベクトルの長さを求める
float Vector3::length(void) const
{
    return std::sqrtf(x * x + y * y + z * z);
}

// ベクトルを正規化する(単位ベクトルに)する
Vector3 Vector3::normalize(void) const
{
    float len{ length() };
    if (len != 0) {
        return *this / len;
    }
    return *this;
}

// 内積を求める
float Vector3::dot(const Vector3& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

// 外積を求める
Vector3 Vector3::cross(const Vector3& v) const
{
    return Vector3{ y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x };
}

// 単項演算子 +
Vector3 Vector3::operator+() const
{
    return *this;
}

// 単項演算子 -
Vector3 Vector3::operator-() const
{
    return Vector3{ -x, -y, -z };
}

Vector3& Vector3::operator=(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vector3& Vector3::operator=(const DirectX::XMFLOAT3& xmf3)
{
    x = xmf3.x;
    y = xmf3.y;
    z = xmf3.z;
    return *this;
}

// 代入演算子 +=
Vector3& Vector3::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}
// 代入演算子 -=
Vector3& Vector3::operator-=(const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}
// 代入演算子 *=
Vector3& Vector3::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}
// 代入演算子 /=
Vector3& Vector3::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

bool Vector3::operator==(const Vector3& v)
{
    return x == v.x && y == v.y && z == v.z;
}

// 2項演算子 +
const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
    Vector3 temp{ v1 };
    return temp += v2;
}
// 2項演算子 -
const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
    Vector3 temp{ v1 };
    return temp -= v2;
}
const Vector3 operator*(const Vector3& v, float s)
{
    Vector3 temp{ v };
    return temp *= s;
}
const Vector3 operator/(const Vector3& v, float s)
{
    Vector3 temp{ v };
    return temp /= s;
}

const Vector3 lerp(const Vector3& start, const Vector3& end, const float t)
{
    return start * (1.0f - t) + end * t;
}

const Vector3 bezier2(const Vector3& start, const Vector3& controlPoint, const Vector3& end, const float t)
{
    return lerp(lerp(start, controlPoint, t), lerp(controlPoint, end, t), t);
}

const Vector3 bezier3(const Vector3& start, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& end, const float t)
{
    return lerp(
        lerp(lerp(start, controlPoint1, t), lerp(controlPoint1, controlPoint2, t), t),	// start
        lerp(lerp(controlPoint1, controlPoint2, t), lerp(controlPoint2, end, t), t),	// end	
        t);
}

Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float_t t)
{
    size_t n = points.size() - 2;

    if (startIndex > n) return points[n];
    if (startIndex < 1) return points[1];

    Vector3 p0 = points[startIndex - 1];
    Vector3 p1 = points[startIndex];
    Vector3 p2 = points[startIndex + 1];
    Vector3 p3 = points[startIndex + 2];

    Vector3 position = (
        p1 * 2 +
        (-p0 + p2) * t +
        (p0 * 2 - p1 * 5 + p2 * 4 - p3) * std::powf(t, 2) +
        (-p0 + p1 * 3 - p2 * 3 + p3) * std::powf(t, 3)
        ) * 0.5f;

    return position;
}
