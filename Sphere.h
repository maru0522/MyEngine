#pragma once
#include "IPrimitive.h"
#include "Vector3.h"

namespace Primitive
{
    struct Sphere : public IPrimitive
    {
        Sphere(void) : IPrimitive(Shape::SPHERE) {}
        Sphere(const Vector3& arg_center,float arf_radius) 
            : IPrimitive(Shape::SPHERE), center(arg_center),radius(arf_radius) {}

        // 変数
        Vector3 center{ 0, 0, 0 };
        float radius{ 0.f };
    };
}