#pragma once
#include <vector>

class Vector3
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����
    float z{};	// z����

    // �֐�
    Vector3(void) = default;
    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    float length(void) const;                 // �m����(����)�����߂�
    Vector3 normalize(void) const;            // ���K������
    float dot(const Vector3& v) const;        // ���ς����߂�
    Vector3 cross(const Vector3& v) const;    // �O�ς����߂�

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
};

// 2���I�o��
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator/(const Vector3& v, float s);

namespace Math {
    namespace Vector {
        float Length(const Vector3& v);
        Vector3 Normalize(const Vector3& v);
        float Dot(const Vector3& v1, const Vector3& v2);
        Vector3 Cross(const Vector3& v1, const Vector3& v2);

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