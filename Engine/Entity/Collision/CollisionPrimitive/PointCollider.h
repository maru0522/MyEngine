#pragma once
#include "ICollider.h"
#include "Point.h"
/**
 * @file PointCollider.h
 * @brief Pointコライダーについてのクラスが宣言されたファイル
 */

namespace CollisionPrimitive
{
    struct PointCollider : public ICollider, public Primitive::Point
    {
    public:
        //>> 関数
        PointCollider(void) : PointCollider("void") {};
        PointCollider(const std::string& arg_id);
        virtual ~PointCollider(void) override = default;

        bool Dispatch(ICollider* other) override { return other->Col(this); }

    private:
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
        bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
        bool Col(CollisionPrimitive::RayCollider* arg_ray) override;
    };
}

