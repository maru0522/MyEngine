#pragma once

struct Vector2 final
{
public:
    // 変数
    float x{};	// x成分
    float y{};	// y成分

    // 関数
    Vector2(void) = default;
    constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}

    Vector2 Normalize(void) const;            // 正規化する
    float Cross(const Vector2& v) const;      // 外積を求める
    float Dot(const Vector2& v) const;        // 内積を求める
    float Length(void) const;                 // ノルム(長さ)を求める
    bool IsNonZero(void) const;               // xyのどれか1つでも値が入っているか

    // 単項オバロ
    Vector2 operator+() const;
    Vector2 operator-() const;

    // 代入オバロ
    Vector2& operator=(const Vector2& v);
    Vector2& operator+=(const Vector2& v);
    Vector2& operator-=(const Vector2& v);
    Vector2& operator*=(float s);
    Vector2& operator/=(float s);

    // 比較オバロ
    bool operator==(const Vector2& v);
};

// 2項オバロ
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator/(const Vector2& v, float s);

namespace Math {
    namespace Vec2 {
        Vector2 Normalize(const Vector2& v);
        float Cross(const Vector2& v1, const Vector2& v2);
        float Dot(const Vector2& v1, const Vector2& v2);
        float Length(const Vector2& v);

        Vector2 RotateVector(const Vector2& v, float rad);
    }
}