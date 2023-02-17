#pragma once
#include <DirectXMath.h>

class Vector4
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����
    float z{};	// z����
    float w{};	// w����

    // �֐�
    Vector4(void) = default;
    Vector4(const Vector4&) = default;
    constexpr Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

    float length(void) const;                 // �m����(����)�����߂�
    Vector4 normalize(void) const;            // ���K������
    float dot(const Vector4& v) const;        // ���ς����߂�
    Vector4 cross(const Vector4& v) const;    // �O�ς����߂� ����`����ĂȂ����߂킩���

    // �P���I�o��
    Vector4 operator+() const;
    Vector4 operator-() const;

    // ����I�o��
    Vector4& operator=(const Vector4& v);
    Vector4& operator+=(const Vector4& v);
    Vector4& operator-=(const Vector4& v);
    Vector4& operator*=(float s);
    Vector4& operator/=(float s);

    // ��r�I�o��
    bool operator==(const Vector4& v);
};

// 2���I�o��
const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator/(const Vector4& v, float s);