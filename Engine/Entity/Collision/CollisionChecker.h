#pragma once
#include "CollisionPrimitive.h"

namespace CollisionChecker
{
    // ‹…
    const bool SphereToSphere(const CollisionPrimitive::SphereCollider& s1, const CollisionPrimitive::SphereCollider& s2);

    // ‹…‚Æ•½–Ê
    const bool SphereToPlane(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PlaneCollider& p, Vector3* intersection = nullptr);

    // •½–Ê‚Æ•½–Ê
    const bool PlaneToPlane(const CollisionPrimitive::PlaneCollider& p1, const CollisionPrimitive::PlaneCollider& p2);
}