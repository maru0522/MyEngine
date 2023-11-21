#include "RayCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::RayCollider::RayCollider(const std::string& arg_id)
    : Ray(), ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Shpere->InitOther();
    bool isHit = CollisionChecker::SphereToRay(*arg_Shpere, *this);

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

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::RayCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}