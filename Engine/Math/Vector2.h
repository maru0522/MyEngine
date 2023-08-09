#pragma once

struct Vector2 final
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����

    // �֐�
    Vector2(void) = default;
    constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}

    Vector2 Normalize(void) const;            // ���K������
    float Cross(const Vector2& v) const;      // �O�ς����߂�
    float Dot(const Vector2& v) const;        // ���ς����߂�
    float Length(void) const;                 // �m����(����)�����߂�
    bool IsNonZero(void) const;               // xy�̂ǂꂩ1�ł��l�������Ă��邩

    // �P���I�o��
    Vector2 operator+() const;
    Vector2 operator-() const;

    // ����I�o��
    Vector2& operator=(const Vector2& v);
    Vector2& operator+=(const Vector2& v);
    Vector2& operator-=(const Vector2& v);
    Vector2& operator*=(float s);
    Vector2& operator/=(float s);

    // ��r�I�o��
    bool operator==(const Vector2& v);
};

// 2���I�o��
const Vector2 operator+(const Vector2& v1, const Vector2& v2);
const Vector2 operator-(const Vector2& v1, const Vector2& v2);
const Vector2 operator*(const Vector2& v, float s);
const Vector2 operator/(const Vector2& v, float s);

namespace Math {
    namespace Vec2 {
        Vector2 Normalize(const Vector2& v);
        float Cross(const Vector2& v1, const Vector2& v2);
        float Dot(const Vector2& v1, const Vector2& v2);
        float Length(const Vector2& v);

        Vector2 RotateVector(const Vector2& v, float rad);
    }
}