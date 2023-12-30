#pragma once
#include "Vector3.h"
#include "Matrix4.h"
/**
 * @file Quaternion.h
 * @brief クオータニオンについてのクラスが宣言されたファイル
 */

class Quaternion
{
public: // 変数
    float x{};
    float y{};
    float z{};
    float w{};

    // 関数
    Quaternion(void) = default;
    constexpr Quaternion(const Vector3& v) noexcept : x(v.x), y(v.y), z(v.z), w(0) {}
    constexpr Quaternion(const Vector3& v, float w) noexcept : x(v.x), y(v.y), z(v.z), w(w) {}
    constexpr Quaternion(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

    Quaternion Normalize(void) const;
    Quaternion Vec3Normalize(void) const;
    Quaternion Conjugate(void) const;
    Quaternion Inverse(void) const;
    float Dot(const Quaternion& q) const;
    float Length(void) const;

    // 単項演算子オーバーロード
    Quaternion operator+() const;
    Quaternion operator-() const;

    // 代入演算子オーバーロード
    Quaternion& operator+=(const Quaternion& q);
    Quaternion& operator-=(const Quaternion& q);
    Quaternion& operator*=(float s);
    Quaternion& operator/=(float s);
};

// 2項演算子オーバーロード
const Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator*(const Quaternion& q, float s);
const Quaternion operator/(const Quaternion& q, float s);

namespace Math {
    // FuncのF
    namespace QuaternionF {
        inline Quaternion Identity(void) { return Quaternion(0, 0, 0, 1); }

        // 任意軸回転を表すクォータニオン
        Quaternion MakeAxisAngle(const Vector3& axis, float radian);
        // オイラー角からクォータニオン
        Quaternion EulerToQuaternion(const Vector3& eular);

        // ベクトルをクォータニオンで回転させた結果のベクトルを求める
        Vector3 RotateVector(const Vector3& v, const Quaternion& q);

        // クォータニオンから回転行列を求める
        Matrix4 MakeRotateMatrix(const Quaternion& q);
        // 3つのクォータニオンから回転行列を求める
        Matrix4 MakeRotateMatrix3(const Quaternion& forward, const Quaternion& right, const Quaternion& up);

        //球面線形補間
        Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
        // uからvへの回転を生成
        Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);
    }
}
