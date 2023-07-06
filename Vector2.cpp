#include "Vector2.h"
#include <cmath>

float Vector2::length(void) const
{
    return std::sqrtf(x * x + y * y);
}

bool Vector2::IsNonZero(void) const
{
    return x || y;
}

Vector2 Vector2::normalize(void) const
{
    float len{ length() };
    if (len != 0) {
        return *this / len;
    }
    return *this;
}

float Vector2::dot(const Vector2& v) const
{
    return x * v.x + y * v.y;
}

float Vector2::cross(const Vector2& v) const
{
    return x * v.y - y * v.x;
}

Vector2 Vector2::operator+() const
{
    return *this;
}

Vector2 Vector2::operator-() const
{
    return Vector2{ -x, -y };
}

Vector2& Vector2::operator=(const Vector2& v)
{
    x = v.x;
    y = v.y;
    return *this;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2& Vector2::operator*=(float s)
{
    x *= s;
    y *= s;
    return *this;
}

Vector2& Vector2::operator/=(float s)
{
    x /= s;
    y /= s;
    return *this;
}

bool Vector2::operator==(const Vector2& v)
{
    return x == v.x && y == v.y;
}

const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
    Vector2 temp{ v1 };
    return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
    Vector2 temp{ v1 };
    return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s)
{
    Vector2 temp{ v };
    return temp *= s;
}

const Vector2 operator/(const Vector2& v, float s)
{
    Vector2 temp{ v };
    return temp /= s;
}

Vector2 Math::Vec2::Normalize(const Vector2& v)
{
    float len{ Length(v)};
    if (len != 0) {
        return v / len;
    }
    return v;
}

float Math::Vec2::Cross(const Vector2& v1, const Vector2& v2)
{
    return v1.x * v2.y - v1.y * v2.x;
}

float Math::Vec2::Dot(const Vector2& v1, const Vector2& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float Math::Vec2::Length(const Vector2& v)
{
    return std::sqrtf(v.x * v.x + v.y * v.y);
}
