#pragma once
#include "IPrimitive.h"
#include "Vector3.h"
/**
 * @file OBB.h
 * @brief OBBについてのクラスが宣言されたファイル
 */

namespace Primitive
{
    struct OBB : public IPrimitive
    {
        //>> 関数
        OBB(void) : IPrimitive(Shape::OBB) {}
        OBB(const Vector3& arg_center, const Vector3* const arg_orien[3], const Vector3& arg_radius);
        virtual ~OBB(void) override = default;

        //>> 変数
        Vector3 center{ 0, 0, 0 };           // 中心点
        Vector3 orientations[3]{ {},{},{} }; // 各軸
        Vector3 radius{ 0, 0, 0 };           // 半径
        Vector3 inter{ 0, 0, 0 };            // 交接点
    };
}

