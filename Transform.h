#pragma once
#include <Vector3.h>

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