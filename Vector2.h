#include <DirectXMath.h>

class Vector2
{
public:
    // 変数
    float x{};	// x成分
    float y{};	// y成分

    // 関数
    Vector2(void); // 零ベクトルとする
    Vector2(float x, float y); // x成分, y成分 を指定しての生成
    Vector2(DirectX::XMFLOAT2 xmf2); // XMFLOAT2でそのまま生成できるように

    float length(void) const;                 // ノルム(長さ)を求める
    Vector2 normalize(void) const;            // 正規化する
    float dot(const Vector2& v) const;        // 内積を求める
    float cross(const Vector2& v) const;      // 外積を求める

    // 単項オバロ
    Vector2 operator+() const;
    Vector2 operator-() const;

    // 代入オバロ
    Vector2& operator=(const Vector2& v);
    Vector2& operator=(const DirectX::XMFLOAT2& xmf2);
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