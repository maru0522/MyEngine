#pragma once
#include "IPrimitive.h"
#include "Vector3.h"

namespace Primitive
{
    struct Ray : public IPrimitive
    {
        //>> 関数
        Ray(void) : IPrimitive(Shape::RAY) {}
        Ray(const Vector3& arg_start, const Vector3& arg_dir) 
            : IPrimitive(Shape::RAY), start(arg_start),dir(arg_dir) {}
        virtual ~Ray(void) override = default;

        //>> 変数
        Vector3 start{ 0, 0, 0 };
        Vector3 dir{ 0, 0, 0 };
    };
}

