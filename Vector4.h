#pragma once
#include <DirectXMath.h>

class Vector4
{
public:
    // 変数
    float x{};	// x成分
    float y{};	// y成分
    float z{};	// z成分
    float w{};	// w成分

    // 関数
    Vector4(void); // 零ベクトルとする
    Vector4(float x, float y, float z, float w); // x成分, y成分, z成分, w成分 を指定しての生成
    Vector4(DirectX::XMFLOAT4 xmf4); // XMFLOAT4でそのまま生成できるように

    float length(void) const;                 // ノルム(長さ)を求める
    Vector4 normalize(void) const;            // 正規化する
    float dot(const Vector4& v) const;        // 内積を求める
    Vector4 cross(const Vector4& v) const;    // 外積を求める ※定義されてないためわからんち

    // 単項オバロ
    Vector4 operator+() const;
    Vector4 operator-() const;

    // 代入オバロ
    Vector4& operator=(const Vector4& v);
    Vector4& operator=(const DirectX::XMFLOAT4& xmf4);
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