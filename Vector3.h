#pragma once
#include <vector>
#include <DirectXMath.h>

class Vector3
{
public:
    // 変数
    float x{};	// x成分
    float y{};	// y成分
    float z{};	// z成分

    // 関数
    Vector3(void); // 零ベクトルとする
    Vector3(float x, float y, float z); // x成分, y成分, z成分 を指定しての生成
    Vector3(DirectX::XMFLOAT3 xmf3); // XMFLOAT3でそのまま生成できるように

    float length(void) const;                 // ノルム(長さ)を求める
    Vector3 normalize(void) const;            // 正規化する
    float dot(const Vector3& v) const;        // 内積を求める
    Vector3 cross(const Vector3& v) const;    // 外積を求める

    // 単項オバロ
    Vector3 operator+() const;
    Vector3 operator-() const;

    // 代入オバロ
    Vector3& operator=(const Vector3& v);
    Vector3& operator=(const DirectX::XMFLOAT3& xmf3);
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(float s);
    Vector3& operator/=(float s);

    // 比較オバロ
    bool operator==(const Vector3& v);
};

// 2項オバロ
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator/(const Vector3& v, float s);

// 補完関数
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t); // 線形補完（1次関数補完）
const Vector3 bezier2(const Vector3& start, const Vector3& controlPoint, const Vector3& end, const float t); // ベジエ（2次関数補完）
const Vector3 bezier3(const Vector3& start, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& end, const float t); // ベジエ（3次関数補完）

Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float_t t);