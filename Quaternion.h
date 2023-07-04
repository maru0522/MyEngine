#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class Quaternion
{
public: // �ϐ�
    float x{};
    float y{};
    float z{};
    float w{};

    // �֐�
    Quaternion(void) = default;
    constexpr Quaternion(const Vector3& v) noexcept : x(v.x), y(v.y), z(v.z), w(0) {}
    constexpr Quaternion(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

    Quaternion Conjugate(void) const;
    float Norm(void) const;
    Quaternion Normalize(void) const;
    float Dot(const Quaternion& q) const;
    Quaternion Inverse(void) const;

    // �P�����Z�q�I�[�o�[���[�h
    Quaternion operator+() const;
    Quaternion operator-() const;

    // ������Z�q�I�[�o�[���[�h
    Quaternion& operator+=(const Quaternion& q);
    Quaternion& operator-=(const Quaternion& q);
    Quaternion& operator*=(float s);
    Quaternion& operator/=(float s);
};

// 2�����Z�q�I�[�o�[���[�h
const Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
const Quaternion operator*(const Quaternion& q, float s);
const Quaternion operator/(const Quaternion& q, float s);

namespace Math {
    // Func��F
    namespace QuaternionF {
        inline Quaternion Identity(void) { return Quaternion(0, 0, 0, 1); }

        // �C�ӎ���]��\���N�H�[�^�j�I��
        Quaternion MakeAxisAngle(const Vector3& axis, float radian);
        // �I�C���[�p����N�H�[�^�j�I��
        Quaternion EulerToQuaternion(const Vector3& eular);

        // �x�N�g�����N�H�[�^�j�I���ŉ�]���������ʂ̃x�N�g�������߂�
        Vector3 RotateVector(const Vector3& v, const Quaternion& q);

        // �N�H�[�^�j�I�������]�s������߂�
        Matrix4 MakeRotateMatrix(const Quaternion& q);
        // 3�̃N�H�[�^�j�I�������]�s������߂�
        Matrix4 MakeRotateMatrix3(const Quaternion& q1, const Quaternion& q2, const Quaternion& q3);

        //���ʐ��`���
        Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
        // u����v�ւ̉�]�𐶐�
        Quaternion DirectionToDirection(const Vector3& u, const Vector3& v);
    }
}