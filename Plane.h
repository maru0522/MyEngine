#pragma once
#include "IPrimitive.h"
#include "Vector3.h"

namespace Primitive
{
    class Plane : public IPrimitive
    {
        Plane(void) : IPrimitive(Shape::PLANE) {}
        Plane(const Vector3& arg_normal, float arf_distance)
            : IPrimitive(Shape::PLANE), normal(arg_normal), distance(arf_distance) {}
        virtual ~Plane(void) override = default;

        // 変数
        Vector3 normal{ 0, 1, 0 };
        float distance{ 0.f };
    };
}

