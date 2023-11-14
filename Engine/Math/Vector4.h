#pragma once

struct Vector4 final
{
public:
    // 変数
    float x{};	// x成分
    float y{};	// y成分
    float z{};	// z成分
    float w{};	// w成分

    // 関数
    Vector4(void) = default;
    constexpr Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

    Vector4 Normalize(void) const;            // 正規化する
    float Dot(const Vector4& v) const;        // 内積を求める
    float Length(void) const;                 // ノルム(長さ)を求める
    bool isNonZero(void) const;               // xyzwのどれか1つでも値が入っているか

    // 単項オバロ
    Vector4 operator+() const;
    Vector4 operator-() const;

    // 代入オバロ
    Vector4& operator=(const Vector4& v);
    Vector4& operator+=(const Vector4& v);
    Vector4& operator-=(const Vector4& v);
    Vector4& operator*=(float s);
    Vector4& operator/=(float s);

    // 比較オバロ
    bool operator==(const Vector4& v);
};

// 2項オバロ
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator/(const Vector4& v, float s);
