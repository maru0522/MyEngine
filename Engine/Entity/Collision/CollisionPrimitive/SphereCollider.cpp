#include "SphereCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::SphereCollider::SphereCollider(const std::string& arg_id)
    : Sphere(),ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 球と球

    bool isHit = CollisionChecker::SphereToSphere(*this, *arg_Shpere);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Shpere->GetCallback_onCollision()) { arg_Shpere->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 球と平面

    bool isHit = CollisionChecker::SphereToPlane(*this, *arg_Plane);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Plane);
        arg_Plane->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Plane->GetCallback_onCollision()) { arg_Plane->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 球と点
    bool isHit = CollisionChecker::SphereToPoint(*this, *arg_Point);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Point);
        arg_Point->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_Point->GetCallback_onCollision()) { arg_Point->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 球と直方体
    bool isHit = CollisionChecker::SphereToAABB(*this, *arg_AABB, &arg_AABB->inter);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_AABB);
        arg_AABB->SetOther(this);

        // nullチェックと衝突処理の実行
        if (GetCallback_onCollision()) { GetCallback_onCollision()(); }
        if (arg_AABB->GetCallback_onCollision()) { arg_AABB->GetCallback_onCollision()(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}