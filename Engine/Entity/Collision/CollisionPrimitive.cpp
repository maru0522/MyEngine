#include "CollisionChecker.h"
#include "CollisionPrimitive.h"

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ‹…
    bool isHit = CollisionChecker::SphereToSphere(*this, *arg_Shpere);

    if (isHit)
    {
        colInfo_ = { arg_Shpere->center, arg_Shpere->radius, arg_Shpere->GetID() };
        arg_Shpere->SetColInfo(CollisionInfo{ center,radius,id_ });

        // callback
        //onTrigger_();
        //arg_Shpere->GetOnTrigger()();

        onCollision_();
        arg_Shpere->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // •½–Ê
    bool isHit = CollisionChecker::SphereToPlane(*this, *arg_Plane);

    if (isHit)
    {
        colInfo_ = { arg_Plane->normal, arg_Plane->distance, arg_Plane->GetID() };
        arg_Plane->SetColInfo(CollisionInfo{ center,radius,id_ });

        // callback
        //onTrigger_();
        //arg_Plane->GetOnTrigger()();

        onCollision_();
        arg_Plane->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{ // “_
    bool isHit = CollisionChecker::SphereToPoint(*this, *arg_Point);

    if (isHit)
    {
        colInfo_.v = arg_Point->pos;
        colInfo_.id = arg_Point->GetID();
        arg_Point->SetColInfo(CollisionInfo{ center,radius,id_ });

        onCollision_();
        arg_Point->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{ // ’¼•û‘Ì
    bool isHit = CollisionChecker::SphereToAABB(*this, *arg_AABB);

    if (isHit)
    {
        colInfo_ = { arg_AABB->center, arg_AABB->radius.x, arg_AABB->GetID() };
        colInfo_.f2 = arg_AABB->radius.y;
        colInfo_.f3 = arg_AABB->radius.z;
        arg_AABB->SetColInfo(CollisionInfo{ center,radius,id_ });

        onCollision_();
        arg_AABB->GetOnCollision()();
    }

    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // •½–Ê
    bool isHit = CollisionChecker::PlaneToPlane(*this, *arg_Plane);

    if (isHit)
    {
        colInfo_ = { arg_Plane->normal, arg_Plane->distance, arg_Plane->GetID() };
        arg_Plane->SetColInfo(CollisionInfo{ normal,distance,id_ });

        // callback
        onCollision_();
        arg_Plane->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ‹…
    bool isHit = CollisionChecker::SphereToPlane(*arg_Shpere, *this);

    if (isHit)
    {
        colInfo_ = { arg_Shpere->center, arg_Shpere->radius, arg_Shpere->GetID() };
        arg_Shpere->SetColInfo(CollisionInfo{ normal,distance,id_ });

        // callback
        onCollision_();
        arg_Shpere->GetOnCollision()();
    }

    return isHit;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{
    if (arg_Shpere)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::AABBCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{
    if (arg_Shpere)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{
    if (arg_Plane)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::PointCollider* arg_Point)
{
    if (arg_Point)
    {

    }
    return false;
}

bool CollisionPrimitive::PointCollider::Col(CollisionPrimitive::AABBCollider* arg_AABB)
{
    if (arg_AABB)
    {

    }
    return false;
}
