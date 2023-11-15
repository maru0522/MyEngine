#pragma once
#include "ICollider.h"
#include "AABB.h"

namespace CollisionPrimitive
{
    class AABBCollider : public ICollider, public Primitive::AABB
    {
    public:
        //>> 関数
        AABBCollider(void) : AABBCollider("void") {};
        AABBCollider(const std::string& arg_id);
        virtual ~AABBCollider(void) override = default;

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

