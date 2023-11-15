#include "PlaneCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::PlaneCollider::PlaneCollider(const std::string& arg_id)
    : Plane(),ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 平面と平面
    bool isHit = CollisionChecker::PlaneToPlane(*this, *arg_Plane);

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

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 平面と球
    bool isHit = CollisionChecker::SphereToPlane(*arg_Shpere, *this);

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

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 平面と点
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 平面と直方体
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}