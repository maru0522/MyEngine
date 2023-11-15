#pragma once
#include "IPrimitive.h"
#include "Vector3.h"

namespace Primitive
{
    struct AABB : public IPrimitive
    {
        //>> 関数
        AABB(void) : IPrimitive(Shape::AABB) {}
        AABB(const Vector3& arg_center, const Vector3& arg_radius)
            : IPrimitive(Shape::AABB), center(arg_center), radius(arg_radius) {}
        virtual ~AABB(void) override = default;

        //>> 変数
        Vector3 center{ 0, 0, 0 };
        Vector3 radius{ 0, 0, 0 };
        Vector3 inter{ 0, 0, 0 }; // 交接点
    };
}

