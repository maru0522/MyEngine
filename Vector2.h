#include <DirectXMath.h>

class Vector2
{
public:
    // �ϐ�
    float x{};	// x����
    float y{};	// y����

    // �֐�
    Vector2(void); // ��x�N�g���Ƃ���
    Vector2(float x, float y); // x����, y���� ���w�肵�Ă̐���
    Vector2(DirectX::XMFLOAT2 xmf2); // XMFLOAT2�ł��̂܂ܐ����ł���悤��

    float length(void) const;                 // �m����(����)�����߂�
    Vector2 normalize(void) const;            // ���K������
    float dot(const Vector2& v) const;        // ���ς����߂�
    float cross(const Vector2& v) const;      // �O�ς����߂�

    // �P���I�o��
    Vector2 operator+() const;
    Vector2 operator-() const;

    // ����I�o��
    Vector2& operator=(const Vector2& v);
    Vector2& operator=(const DirectX::XMFLOAT2& xmf2);
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