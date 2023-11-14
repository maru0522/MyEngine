#include "Vector4.h"
#include <cmath>

float Vector4::Length(void) const
{
    return std::sqrtf(x * x + y * y + z * z + w * w);
}

bool Vector4::isNonZero(void) const
{
    return x || y || z || w;
}

Vector4 Vector4::Normalize(void) const
{
    float len{ Length() };
    if (len != 0) {
        return *this / len;
    }
    return *this;
}

float Vector4::Dot(const Vector4& v) const
{
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

Vector4 Vector4::operator+() const
{
    return *this;
}

Vector4 Vector4::operator-() const
{
    return Vector4{ -x, -y, -z, -w };
}

Vector4& Vector4::operator=(const Vector4& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

Vector4& Vector4::operator+=(const Vector4& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

Vector4& Vector4::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

Vector4& Vector4::operator/=(float s)
{
    x /= s;
    y /= s;
    z /= s;
    w /= s;
    return *this;
}

bool Vector4::operator==(const Vector4& v)
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

const Vector4 operator+(const Vector4& v1, const Vector4& v2)
{
    Vector4 temp{ v1 };
    return temp += v2;
}

const Vector4 operator-(const Vector4& v1, const Vector4& v2)
{
    Vector4 temp{ v1 };
    return temp -= v2;
}

const Vector4 operator*(const Vector4& v, float s)
{
    Vector4 temp{ v };
    return temp *= s;
}

const Vector4 operator/(const Vector4& v, float s)
{
    Vector4 temp{ v };
    return temp /= s;
}
