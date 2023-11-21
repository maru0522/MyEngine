#include "AABBCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::AABBCollider::AABBCollider(const std::string& arg_id)
    : AABB(),ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 直方体と球

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Shpere->InitOther();
    bool isHit = CollisionChecker::SphereToAABB(*arg_Shpere, *this, &this->inter);

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

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 直方体と平面
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 直方体と点

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Point->InitOther();
    bool isHit = CollisionChecker::AABBToPoint(*this, *arg_Point);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Point);
        arg_Point->SetOther(this);

        // nullチェックと衝突処理の実行
        if (callback_onCollision_) { callback_onCollision_(); }
        if (arg_Point->callback_onCollision_) { arg_Point->callback_onCollision_(); }
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 直方体と直方体

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_AABB->InitOther();
    bool isHit = CollisionChecker::AABBToAABB(*this, *arg_AABB);

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

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::OBBCollider* arg_OBB)
{
    if (arg_OBB)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::RayCollider* arg_ray)
{
    if (arg_ray)
    {

    }
    return false;
}