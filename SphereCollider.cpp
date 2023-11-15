#include "SphereCollider.h"

CollisionPrimitive::SphereCollider::SphereCollider(const std::string& arg_id)
    : Sphere(),ICollider()
{
    id_ = arg_id;
}

struct SphereCollider final : public ICollider
{
    // 関数
    SphereCollider(void)
    {
        shape_ = Shape::SPHERE;
    }
    SphereCollider(const std::string& id)
    {
        id_ = id;
        shape_ = Shape::SPHERE;
    }

    inline bool Dispatch(ICollider* other) override { return other->Col(this); }

    // 変数
    Vector3 center{ 0, 0, 0 };
    float radius{ 0.f };

private:
    bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
    bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
    bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
    bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
    bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
    bool Col(CollisionPrimitive::RayCollider* arg_ray) override;
};