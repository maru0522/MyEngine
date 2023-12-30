#pragma once
#include "ICollider.h"
#include "Plane.h"
/**
 * @file PlaneCollider.h
 * @brief Planeコライダーについてのクラスが宣言されたファイル
 */

namespace CollisionPrimitive
{
    struct PlaneCollider : public ICollider, public Primitive::Plane
    {
    public:
        //>> 関数
        PlaneCollider(void) : PlaneCollider("void") {};
        PlaneCollider(const std::string& arg_id);
        virtual ~PlaneCollider(void) override = default;

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

