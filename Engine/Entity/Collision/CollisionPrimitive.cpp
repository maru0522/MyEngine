#include "CollisionChecker.h"
#include "CollisionPrimitive.h"

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 球と球
    bool isHit = CollisionChecker::SphereToSphere(*this, *arg_Shpere);

    if (isHit)
    {
        other_ = arg_Shpere;
        arg_Shpere->SetOther(this);

        // callback
        //onTrigger_();
        //arg_Shpere->GetOnTrigger()();

        if (onCollision_) { onCollision_(); }
        if (onTrigger_) { onTrigger_(); }
        if (onRelease_) { onRelease_(); }
        arg_Shpere->GetOnCollision()();
        //AllOnFunction();
        //arg_Shpere->AllOnFunction();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 球と平面
    bool isHit = CollisionChecker::SphereToPlane(*this, *arg_Plane);

    if (isHit)
    {
        other_ = arg_Plane;
        arg_Plane->SetOther(this);

        // callback
        //onTrigger_();
        //arg_Plane->GetOnTrigger()();

        onCollision_();
        arg_Plane->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // 球と点
    bool isHit = CollisionChecker::SphereToPoint(*this, *arg_Point);

    if (isHit)
    {
        other_ = arg_Point;
        arg_Point->SetOther(this);

        onCollision_();
        arg_Point->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 球と直方体
    bool isHit = CollisionChecker::SphereToAABB(*this, *arg_AABB);

    if (isHit)
    {
        other_ = arg_AABB;
        arg_AABB->SetOther(this);

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // 平面と平面
    bool isHit = CollisionChecker::PlaneToPlane(*this, *arg_Plane);

    if (isHit)
    {
        other_ = arg_Plane;
        arg_Plane->SetOther(this);

        // callback
        onCollision_();
        arg_Plane->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 平面と球
    bool isHit = CollisionChecker::SphereToPlane(*arg_Shpere, *this);

    if (isHit)
    {
        other_ = arg_Shpere;
        arg_Shpere->SetOther(this);

        // callback
        onCollision_();
        arg_Shpere->GetOnCollision()();
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

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 直方体と球
    bool isHit = CollisionChecker::SphereToAABB(*arg_Shpere, *this);

    if (isHit)
    {
        other_ = arg_Shpere;
        arg_Shpere->SetOther(this);

        onCollision_();
        arg_Shpere->GetOnCollision()();
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
    bool isHit = CollisionChecker::AABBToPoint(*this, *arg_Point);

    if (isHit)
    {
        other_ = arg_Point;
        arg_Point->SetOther(this);

        onCollision_();
        arg_Point->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // 直方体と直方体
    bool isHit = CollisionChecker::AABBToAABB(*this, *arg_AABB);

    if (isHit)
    {
        other_ = arg_AABB;
        arg_AABB->SetOther(this);

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // 点と球
    bool isHit = CollisionChecker::SphereToPoint(*arg_Shpere, *this);

    if (isHit)
    {
        other_ = arg_Shpere;
        arg_Shpere->SetOther(this);

        onCollision_();
        arg_Shpere->GetOnCollision()();
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
    bool isHit = CollisionChecker::AABBToPoint(*arg_AABB, *this);

    if (isHit)
    {
        other_ = arg_AABB;
        arg_AABB->SetOther(this);

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return isHit;
}
