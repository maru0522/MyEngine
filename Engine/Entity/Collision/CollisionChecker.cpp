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

const bool CollisionChecker::SphereToPoint(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PointCollider& p)
{
    return (s.center.x - p.pos.x) * (s.center.x - p.pos.x) +
           (s.center.y - p.pos.y) * (s.center.y - p.pos.y) +
           (s.center.z - p.pos.z) * (s.center.z - p.pos.z) <= s.radius * s.radius;
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

const bool CollisionChecker::SphereToAABB(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::AABBCollider& a, Vector3* intersection)
{
    // AABB内で最も球に近い点
    Vector3 nearest{};
    nearest.x = (std::max)(a.center.x - a.radius.x, (std::min)(s.center.x, a.center.x + a.radius.x));
    nearest.y = (std::max)(a.center.y - a.radius.y, (std::min)(s.center.y, a.center.y + a.radius.y));
    nearest.z = (std::max)(a.center.z - a.radius.z, (std::min)(s.center.z, a.center.z + a.radius.z));
    // nearest == sphere.center の場合、球はAABB内にあるため true

    // 交錯点を記録
    if (intersection) { *intersection = nearest; }

    // SphereToPoint() と同じ
    return (s.center.x - nearest.x) * (s.center.x - nearest.x) +
           (s.center.y - nearest.y) * (s.center.y - nearest.y) +
           (s.center.z - nearest.z) * (s.center.z - nearest.z) <= s.radius * s.radius;
}

const bool CollisionChecker::PlaneToPlane(const CollisionPrimitive::PlaneCollider& p1, const CollisionPrimitive::PlaneCollider& p2)
{
    float dot = Math::Vec3::Dot(p1.normal, p2.normal);

    // 内積が1に近い場合無限平面
    if (std::fabsf(dot - 1.f) < 0.0001f)
    {
        float distDiff = std::fabsf(p1.distance - p2.distance);
        return distDiff < 0.0001f;
    }

    return true;
}

const bool CollisionChecker::AABBToAABB(const CollisionPrimitive::AABBCollider& a1, const CollisionPrimitive::AABBCollider& a2)
{
    Vector3 a1_min = { a1.center.x - a1.radius.x, a1.center.y - a1.radius.y, a1.center.z - a1.radius.z };
    Vector3 a1_max = { a1.center.x + a1.radius.x, a1.center.y + a1.radius.y, a1.center.z + a1.radius.z };
    Vector3 a2_min = { a2.center.x - a2.radius.x, a2.center.y - a2.radius.y, a2.center.z - a2.radius.z };
    Vector3 a2_max = { a2.center.x + a2.radius.x, a2.center.y + a2.radius.y, a2.center.z + a2.radius.z };

    return a1_min.x <= a2_max.x &&
           a1_max.x >= a2_min.x &&
           a1_min.y <= a2_max.y &&
           a1_max.y >= a2_min.y &&
           a1_min.z <= a2_max.z &&
           a1_max.z >= a2_min.z;
}

const bool CollisionChecker::AABBToPoint(const CollisionPrimitive::AABBCollider& a, const CollisionPrimitive::PointCollider& p)
{
    return (a.center.x - a.radius.x <= p.pos.x && p.pos.x <= a.center.x + a.radius.x) &&
           (a.center.y - a.radius.y <= p.pos.y && p.pos.y <= a.center.y + a.radius.y) &&
           (a.center.z - a.radius.z <= p.pos.z && p.pos.z <= a.center.z + a.radius.z);
}

const bool CollisionChecker::PointToPoint(const CollisionPrimitive::PointCollider& p1, const CollisionPrimitive::PointCollider& p2)
{
    return p1.pos == p2.pos;
}
