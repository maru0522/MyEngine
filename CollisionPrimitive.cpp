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
