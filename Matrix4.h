#pragma once
#include "Vector3.h"

class Matrix4
{
public:
    // �֐�
    Matrix4(void) {};
    Matrix4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33);

    // �ϐ�
    float m[4][4]{};
};

namespace Math {
    namespace Matrix {
        // �P�ʍs��
        Matrix4 Identity();

        // �g��k���s��
        Matrix4 Scale(const Vector3& s);

        // ��]�s��
        Matrix4 RotateX(float angle);
        Matrix4 RotateY(float angle);
        Matrix4 RotateZ(float angle);

        // ���s�ړ��s��
        Matrix4 Translate(const Vector3& t);

        // ���W�ϊ�
        Vector3 Transform(const Vector3& v, const Matrix4& m);
    }
}

// ������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// 2�����Z�q�I�[�o�[���[�h
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
const Vector3 operator*(const Vector3& v, const Matrix4& m);