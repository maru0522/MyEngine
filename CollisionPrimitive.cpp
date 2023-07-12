#include "CollisionChecker.h"
#include "CollisionPrimitive.h"

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ‹…
    colInfo_ = { arg_Shpere->center ,arg_Shpere->radius };
    isHit_ = CollisionChecker::SphereToSphere(*this, *arg_Shpere);
    arg_Shpere->SetColInfo(CollisionInfo{ center,radius });
    arg_Shpere->SetIsHit(isHit_);
    return isHit_;
}

bool CollisionPrimitive::SphereCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // •½–Ê
    colInfo_ = { arg_Plane->normal ,arg_Plane->distance };
    isHit_ = CollisionChecker::SphereToPlane(*this, *arg_Plane);
    arg_Plane->SetColInfo(CollisionInfo{ center,radius });
    arg_Plane->SetIsHit(isHit_);
    return isHit_;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::PlaneCollider* arg_Plane)
{ // •½–Ê
    colInfo_ = { arg_Plane->normal ,arg_Plane->distance };
    isHit_ = CollisionChecker::PlaneToPlane(*this, *arg_Plane);
    arg_Plane->SetColInfo(CollisionInfo{ normal,distance });
    arg_Plane->SetIsHit(isHit_);
    return isHit_;
}

bool CollisionPrimitive::PlaneCollider::Col(CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ‹…
    colInfo_ = { arg_Shpere->center ,arg_Shpere->radius };
    isHit_ = CollisionChecker::SphereToPlane(*arg_Shpere, *this);
    arg_Shpere->SetColInfo(CollisionInfo{ normal,distance });
    arg_Shpere->SetIsHit(isHit_);

    return isHit_;
}
