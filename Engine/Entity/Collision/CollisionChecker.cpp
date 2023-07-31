#include "CollisionChecker.h"
#include "Vector3.h"
#include <cmath>

const bool CollisionChecker::SphereToSphere(const CollisionPrimitive::SphereCollider& s1, const CollisionPrimitive::SphereCollider& s2)
{
    return (s2.center.x - s1.center.x) * (s2.center.x - s1.center.x) +
           (s2.center.y - s1.center.y) * (s2.center.y - s1.center.y) +
           (s2.center.z - s1.center.z) * (s2.center.z - s1.center.z) <=
           (s2.radius + s1.radius) * (s2.radius + s1.radius);
}

const bool CollisionChecker::SphereToPlane(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PlaneCollider& p, Vector3* intersection)
{
    float distV = Math::Vec3::Dot(s.center, p.normal.Normalize());
    float dist = distV - p.distance;
    if (std::fabsf(dist) > s.radius) return false;

    if (intersection)
    {
        *intersection = -dist * p.normal.Normalize() + s.center;
    }

    return true;
}

const bool CollisionChecker::PlaneToPlane(const CollisionPrimitive::PlaneCollider& p1, const CollisionPrimitive::PlaneCollider& p2)
{
    float dot = Math::Vec3::Dot(p1.normal, p2.normal);

    // ì‡êœÇ™1Ç…ãﬂÇ¢èÍçáñ≥å¿ïΩñ 
    if (std::fabsf(dot - 1.f) < 0.0001f)
    {
        float distDiff = std::fabsf(p1.distance - p2.distance);
        return distDiff < 0.0001f;
    }

    return true;
}
