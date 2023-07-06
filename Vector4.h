#pragma once
#include <DirectXMath.h>

struct Vector4
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����
    float z{};	// z����
    float w{};	// w����

    // �֐�
    Vector4(void) = default;
    constexpr Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

    Vector4 normalize(void) const;            // ���K������
    float dot(const Vector4& v) const;        // ���ς����߂�
    float length(void) const;                 // �m����(����)�����߂�
    bool isNonZero(void) const;               // xyzw�̂ǂꂩ1�ł��l�������Ă��邩

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