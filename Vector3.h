#pragma once
#include <vector>
#include <DirectXMath.h>

class Vector3
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����
    float z{};	// z����

    // �֐�
    Vector3(void); // ��x�N�g���Ƃ���
    Vector3(float x, float y, float z); // x����, y����, z���� ���w�肵�Ă̐���
    Vector3(DirectX::XMFLOAT3 xmf3); // XMFLOAT3�ł��̂܂ܐ����ł���悤��

    float length(void) const;                 // �m����(����)�����߂�
    Vector3 normalize(void) const;            // ���K������
    float dot(const Vector3& v) const;        // ���ς����߂�
    Vector3 cross(const Vector3& v) const;    // �O�ς����߂�

    // �P���I�o��
    Vector3 operator+() const;
    Vector3 operator-() const;

    // ����I�o��
    Vector3& operator=(const Vector3& v);
    Vector3& operator=(const DirectX::XMFLOAT3& xmf3);
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

// �⊮�֐�
const Vector3 lerp(const Vector3& start, const Vector3& end, const float t); // ���`�⊮�i1���֐��⊮�j
const Vector3 bezier2(const Vector3& start, const Vector3& controlPoint, const Vector3& end, const float t); // �x�W�G�i2���֐��⊮�j
const Vector3 bezier3(const Vector3& start, const Vector3& controlPoint1, const Vector3& controlPoint2, const Vector3& end, const float t); // �x�W�G�i3���֐��⊮�j

Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float_t t);