#pragma once
#include "IPrimitive.h"
#include "Vector3.h"
/**
 * @file Sphere.h
 * @brief Sphereについてのクラスが宣言されたファイル
 */

namespace Primitive
{
    struct Sphere : public IPrimitive
    {
        //>> 関数
        Sphere(void) : IPrimitive(Shape::SPHERE) {}
        Sphere(const Vector3& arg_center,float arf_radius) 
            : IPrimitive(Shape::SPHERE), center(arg_center),radius(arf_radius) {}
        virtual ~Sphere(void) override = default;

        // 変数
        Vector3 center{ 0, 0, 0 };
        float radius{ 0.f };
    };
}