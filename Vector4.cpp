#include "Vector4.h"
#include <cmath>

float Vector4::length(void) const
{
    return std::sqrtf(x * x + y * y + z * z + w * w);
}

Vector4 Vector4::normalize(void) const
{
    float len{ length() };
    if (len != 0) {
        return *this / len;
    }
    return *this;
}

float Vector4::dot(const Vector4& v) const
{
    return x * v.x + y * v.y + z * v.z + w * v.w;
}

Vector4 Vector4::cross(const Vector4& v) const
{
    return Vector4{ y * v.w - w * v.y,
                    z * v.x - x * v.z,
                    w * v.y - y * v.w,
                    x * v.z - z * v.x };
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
