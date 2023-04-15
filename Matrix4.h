#pragma once
#include "Vector3.h"

class Matrix4
{
public:
    // �֐�
    Matrix4(void) = default;
    Matrix4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);

    // �ϐ�
    float m[4][4]{};
};

// ������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// 2�����Z�q�I�[�o�[���[�h
const Matrix4 operator*(const Matrix4& m1, const Matrix4& m2);
const Vector3 operator*(const Vector3& v, const Matrix4& m);

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

        // �]�u�s��
        Matrix4 Transpose(const Matrix4& m);

        // ���W�ϊ�
        Vector3 Transform(const Vector3& v, const Matrix4& m);

        // �r���[�s��
        Matrix4 ViewLookToLH(const Vector3& eyePosition, const Vector3& eyeDirection, const Vector3& upDirection);
        Matrix4 ViewLookAtLH(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& upDirection);

        // �ˉe�s��
        Matrix4 ProjectionOrthoGraphicLH(float window_width, float window_height);
        Matrix4 ProjectionOrthoGraphicLH(float window_width, float window_height, float nearZ, float farZ);
        Matrix4 ProjectionOrthoGraphicLH(float leftEdge, float rightEdge, float bottomEdge, float topEdge, float nearZ, float farZ);

        Matrix4 ProjectionPerspectiveFovLH(float fovY, float aspect, float nearZ, float farZ);
        Matrix4 ProjectionPerspectiveFovLH(float fovY, float window_width, float window_height, float nearZ, float farZ);
    }
}
