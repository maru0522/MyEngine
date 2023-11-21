#include "SphereCollider.h"
#include "CollisionChecker.h"

CollisionPrimitive::SphereCollider::SphereCollider(const std::string& arg_id)
    : Sphere(),ICollider()
{
    id_ = arg_id;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 球と球

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Shpere->InitOther();
    bool isHit = CollisionChecker::SphereToSphere(*this, *arg_Shpere);

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

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 球と平面

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Plane->InitOther();
    bool isHit = CollisionChecker::SphereToPlane(*this, *arg_Plane);

    if (isHit)
    {
        // 接触相手のptrを保存
        SetOther(arg_Plane);
        arg_Plane->SetOther(this);

        // nullチェックと衝突処理の実行
        if (callback_onCollision_) { callback_onCollision_(); }
        if (arg_Plane->callback_onCollision_) { arg_Plane->callback_onCollision_(); }
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 球と点

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_Point->InitOther();
    bool isHit = CollisionChecker::SphereToPoint(*this, *arg_Point);

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

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 球と直方体

    // 互いの前回の接触相手の情報を初期化
    InitOther();
    arg_AABB->InitOther();
    bool isHit = CollisionChecker::SphereToAABB(*this, *arg_AABB, &arg_AABB->inter);

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