#pragma once
#include <vector>

struct Vector3 final
{
public:
    // 変数
    float x{};	// x成分
    float y{};	// y成分
    float z{};	// z成分

    // 関数
    Vector3(void) = default;
    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    Vector3 Normalize(void) const;            // 正規化する
    Vector3 Cross(const Vector3& v) const;    // 外積を求める
    float Dot(const Vector3& v) const;        // 内積を求める
    float Length(void) const;                 // ノルム(長さ)を求める
    bool IsNonZero(void) const;               // xyzのどれか1つでも値が入っているか

    // 単項オバロ
    Vector3 operator+() const;
    Vector3 operator-() const;

    // 代入オバロ
    Vector3& operator=(const Vector3& v);
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(float s);
    Vector3& operator/=(float s);

    // 比較オバロ
    bool operator==(const Vector3& v);
    const bool operator==(const Vector3& v) const;
};

// 2項オバロ
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);

namespace Math {
    namespace Vec3 {
        Vector3 Normalize(const Vector3& v);
        Vector3 Cross(const Vector3& v1, const Vector3& v2);
        float Dot(const Vector3& v1, const Vector3& v2);
        float Length(const Vector3& v);

        Vector3 Resolution(const Vector3& v, const Vector3& axis4Proj);

        // 補完関数
        const Vector3 lerp(const Vector3& start, const Vector3& end, const float t); // 線形補完（1次関数補完）
        const Vector3 bezier2(const Vector3& start, const Vector3& controlPoint, const Vector3& end, const float t); // ベジエ（2次関数補完）
        const Vector3 bezier3(const Vector3& start, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& end, const float t); // ベジエ（3次関数補完）

        Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float_t t);

        inline const Vector3 AxisXIdentity(void) { return Vector3(1, 0, 0); }
        inline const Vector3 AxisYIdentity(void) { return Vector3(0, 1, 0); }
        inline const Vector3 AxisZIdentity(void) { return Vector3(0, 0, 1); }
    }
}