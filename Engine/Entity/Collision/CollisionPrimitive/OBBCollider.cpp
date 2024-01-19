#include "OBBCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::OBBCollider::OBBCollider(const std::string& arg_id)
    : OBB(),ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{// OBBと球
    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Shpere->InitOther();
    bool isHit = CollisionChecker::SphereToOBB(*arg_Shpere, *this, &this->inter);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Shpere);
        arg_Shpere->SetOther(this);

        // nullチェックと衝突処理の実行
        if (callback_onCollision_) { callback_onCollision_(); }
        if (arg_Shpere->callback_onCollision_) { arg_Shpere->callback_onCollision_(); }
    }

    return isHit;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::OBBCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}