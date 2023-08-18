#pragma once
#include <vector>

struct Vector3 final
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����
    float z{};	// z����

    // �֐�
    Vector3(void) = default;
    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    Vector3 Normalize(void) const;            // ���K������
    Vector3 Cross(const Vector3& v) const;    // �O�ς����߂�
    float Dot(const Vector3& v) const;        // ���ς����߂�
    float Length(void) const;                 // �m����(����)�����߂�
    bool IsNonZero(void) const;               // xyz�̂ǂꂩ1�ł��l�������Ă��邩

    // �P���I�o��
    Vector3 operator+() const;
    Vector3 operator-() const;

    // ����I�o��
    Vector3& operator=(const Vector3& v);
    Vector3& operator+=(const Vector3& v);
    Vector3& operator-=(const Vector3& v);
    Vector3& operator*=(float s);
    Vector3& operator/=(float s);

    // ��r�I�o��
    bool operator==(const Vector3& v);
    const bool operator==(const Vector3& v) const;
};

// 2���I�o��
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

        // �⊮�֐�
        const Vector3 lerp(const Vector3& start, const Vector3& end, const float t); // ���`�⊮�i1���֐��⊮�j
        const Vector3 bezier2(const Vector3& start, const Vector3& controlPoint, const Vector3& end, const float t); // �x�W�G�i2���֐��⊮�j
        const Vector3 bezier3(const Vector3& start, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& end, const float t); // �x�W�G�i3���֐��⊮�j

        Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float_t t);

        inline const Vector3 AxisXIdentity(void) { return Vector3(1, 0, 0); }
        inline const Vector3 AxisYIdentity(void) { return Vector3(0, 1, 0); }
        inline const Vector3 AxisZIdentity(void) { return Vector3(0, 0, 1); }
    }
}