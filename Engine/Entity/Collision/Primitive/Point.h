#pragma once
#include "IPrimitive.h"
#include "Vector3.h"

namespace Primitive
{
    struct Point : public IPrimitive
    {
        //>> 関数
        Point(void) : IPrimitive(Shape::POINT) {}
        Point(const Vector3& arg_pos) : IPrimitive(Shape::POINT), pos(arg_pos) {}
        virtual ~Point(void) override = default;

        // 変数
        Vector3 pos{ 0, 0, 0 };
    };
}

