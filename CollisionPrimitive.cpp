#include "CollisionChecker.h"
#include "CollisionPrimitive.h"

bool CollisionPrimitive::SphereCollider::Col(const CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ‹…
    colInfo_ = { arg_Shpere->center ,arg_Shpere->radius };
    isHit_ = CollisionChecker::SphereToSphere(*this, *arg_Shpere);
    return isHit_;
}

bool CollisionPrimitive::SphereCollider::Col(const CollisionPrimitive::PlaneCollider* arg_Plane)
{ // •½–Ê
    colInfo_ = { arg_Plane->normal ,arg_Plane->distance };
    isHit_ = CollisionChecker::SphereToPlane(*this, *arg_Plane);
    return isHit_;
}

bool CollisionPrimitive::PlaneCollider::Col(const CollisionPrimitive::PlaneCollider* arg_Plane)
{ // •½–Ê
    colInfo_ = { arg_Plane->normal ,arg_Plane->distance };
    isHit_ = CollisionChecker::PlaneToPlane(*this, *arg_Plane);
    return isHit_;
}

bool CollisionPrimitive::PlaneCollider::Col(const CollisionPrimitive::SphereCollider* arg_Shpere)
{ // ‹…
    colInfo_ = { arg_Shpere->center ,arg_Shpere->radius };
    isHit_ = CollisionChecker::SphereToPlane(*arg_Shpere, *this);
    return isHit_;
}
