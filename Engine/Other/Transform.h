#pragma once
#include "Vector3.h"
#include "Matrix4.h"

struct Transform final
{
    Transform(void) { Initialize(); }
    Transform(const Vector3& position, const Vector3& rotation, const Vector3& scale)
    {
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    void Initialize(void)
    {
        position = { 0,0,0 };
        rotation = { 0,0,0 };
        scale = { 1,1,1 };
    }
};

struct TransformMatrix final
{
    TransformMatrix(void) { Initialize(); }
    TransformMatrix(const Matrix4& world, const Matrix4& sca, const Matrix4& rot, const Matrix4& pos)
        : mat_world(world), mat_sca(sca), mat_rot(rot), mat_pos(pos) {}

    // 値を初期化する
    void Initialize(void) {
        Matrix4 identity = Math::Mat4::Identity();

        mat_world = identity;
        mat_sca = identity;
        mat_rot = identity;
        mat_pos = identity;
    }
    // 行列をscale,rotate,positionの順に合成する
    void CompositionSRT(void) {
        mat_world *= mat_sca;
        mat_world *= mat_rot;
        mat_world *= mat_pos;
    }

    Matrix4 mat_world;
    Matrix4 mat_sca;
    Matrix4 mat_rot;
    Matrix4 mat_pos;

    // getter
    // 行列から結果的な軸ベクトルを抜き出す。
    Vector3 GetMatAxisX(void) const { return { mat_world.m[0][0],mat_world.m[0][1],mat_world.m[0][2] }; }
    Vector3 GetMatAxisY(void) const { return { mat_world.m[1][0],mat_world.m[1][1],mat_world.m[1][2] }; }
    Vector3 GetMatAxisZ(void) const { return { mat_world.m[2][0],mat_world.m[2][1],mat_world.m[2][2] }; }

    // 行列から平行移動成分を抜き出す
    Vector3 GetMatPos(void) const { return { mat_world.m[3][0],mat_world.m[3][1],mat_world.m[3][2] }; }
};

struct Axis3 final
{
    Vector3 forward;
    Vector3 right;
    Vector3 up;

    Axis3(void) { Initialize(); }
    Axis3(const Vector3& forward, const Vector3& right, const Vector3& up)
    {
        this->forward = forward;
        this->right = right;
        this->up = up;
    }

    void Initialize(void)
    {
        forward = { 0,0,1 };
        right = { 1,0,0 };
        up = { 0,1,0 };
    }
};

struct AxisRot final
{
    Vector3 axis;
    float rad;

    AxisRot(void) : AxisRot(Vector3{}, 0.f) {}
    AxisRot(const Vector3& axis, float rad)
    {
        this->axis = axis;
        this->rad = rad;
    }
};
