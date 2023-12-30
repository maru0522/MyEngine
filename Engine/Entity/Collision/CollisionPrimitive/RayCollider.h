#pragma once
#include "ICollider.h"
#include "Ray.h"
/**
 * @file RayCollider.h
 * @brief Rayコライダーについてのクラスが宣言されたファイル
 */

namespace CollisionPrimitive
{
    struct RayCollider : public ICollider, public Primitive::Ray
    {
    public:
        //>> 関数
        RayCollider(void) : RayCollider("void") {};
        RayCollider(const std::string& arg_id);
        virtual ~RayCollider(void) override = default;

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

