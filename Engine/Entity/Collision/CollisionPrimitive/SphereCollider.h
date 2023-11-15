#pragma once
#include "ICollider.h"
#include "Sphere.h"

namespace CollisionPrimitive
{
    struct SphereCollider : public ICollider, public Primitive::Sphere
    {
    public:
        //>> 関数
        SphereCollider(void) : SphereCollider("void") {};
        SphereCollider(const std::string& arg_id);
        virtual ~SphereCollider(void) override = default;

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

