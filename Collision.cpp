#include "Collision.h"

const bool Collision::SphereToSphere(const CollisionPrimitive::Sphere& s1, const CollisionPrimitive::Sphere& s2)
{
    return (s2.center.x - s1.center.x) * (s2.center.x - s1.center.x) +
           (s2.center.y - s1.center.y) * (s2.center.y - s1.center.y) +
           (s2.center.z - s1.center.z) * (s2.center.z - s1.center.z) <=
           (s2.radius + s1.radius) * (s2.radius + s1.radius);
}
