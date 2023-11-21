#include "PointCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::PointCollider::PointCollider(const std::string& arg_id)
    : Point(), ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 点と球

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Shpere->InitOther();
    bool isHit = CollisionChecker::SphereToPoint(*arg_Shpere, *this);

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

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 点と平面
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 点と点
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 点と直方体

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_AABB->InitOther();
    bool isHit = CollisionChecker::AABBToPoint(*arg_AABB, *this);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_AABB);
        arg_AABB->SetOther(this);

        // nullチェックと衝突処理の実行
        if (callback_onCollision_) { callback_onCollision_(); }
        if (arg_AABB->callback_onCollision_) { arg_AABB->callback_onCollision_(); }
    }

    return isHit;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}