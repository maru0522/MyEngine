#include "CollisionChecker.h"
#include "Vector3.h"
#include "Matrix4.h"
#include <cmath>

const bool CollisionChecker::SphereToSphere(const Primitive::Sphere& s1, const Primitive::Sphere& s2)
{
    return (s2.center.x - s1.center.x) * (s2.center.x - s1.center.x) +
           (s2.center.y - s1.center.y) * (s2.center.y - s1.center.y) +
           (s2.center.z - s1.center.z) * (s2.center.z - s1.center.z) <=
           (s2.radius + s1.radius) * (s2.radius + s1.radius);
}

const bool CollisionChecker::SphereToPoint(const Primitive::Sphere& s, const Primitive::Point& p)
{
    return (s.center.x - p.pos.x) * (s.center.x - p.pos.x) +
           (s.center.y - p.pos.y) * (s.center.y - p.pos.y) +
           (s.center.z - p.pos.z) * (s.center.z - p.pos.z) <= s.radius * s.radius;
}

const bool CollisionChecker::SphereToPlane(const Primitive::Sphere& s, const Primitive::Plane& p, Vector3* intersection)
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

const bool CollisionChecker::SphereToAABB(const Primitive::Sphere& s, const Primitive::AABB& a, Vector3* intersection)
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

const bool CollisionChecker::SphereToOBB(const Primitive::Sphere& s, const Primitive::OBB& o, Vector3* intersection)
{
    Matrix4 obbMat = { o.orientations[0].x,o.orientations[0].y,o.orientations[0].z,0,
                       o.orientations[1].x,o.orientations[1].y,o.orientations[1].z,0,
                       o.orientations[2].x,o.orientations[2].y,o.orientations[2].z,0,
                       o.center.x,o.center.y,o.center.z,1 };

    Matrix4 obbMatInverse = Math::Mat4::Inverse(obbMat);

    Primitive::AABB obbLocal_aabb;
    obbLocal_aabb.center = { 0,0,0 };
    obbLocal_aabb.radius = o.radius;

    Primitive::Sphere obbLocal_sphere;
    obbLocal_sphere.center = Math::Mat4::Transform(s.center, obbMatInverse);
    obbLocal_sphere.radius = s.radius;

    Vector3 nearest{};
    nearest.x = (std::max)(obbLocal_aabb.center.x - obbLocal_aabb.radius.x, (std::min)(s.center.x, obbLocal_aabb.center.x + obbLocal_aabb.radius.x));
    nearest.y = (std::max)(obbLocal_aabb.center.y - obbLocal_aabb.radius.y, (std::min)(s.center.y, obbLocal_aabb.center.y + obbLocal_aabb.radius.y));
    nearest.z = (std::max)(obbLocal_aabb.center.z - obbLocal_aabb.radius.z, (std::min)(s.center.z, obbLocal_aabb.center.z + obbLocal_aabb.radius.z));
    // nearest == sphere.center の場合、球はAABB内にあるため true

    if (intersection)
    {
        intersection = intersection;
    }

    // SphereToPoint() と同じ
    return (s.center.x - nearest.x) * (s.center.x - nearest.x) +
           (s.center.y - nearest.y) * (s.center.y - nearest.y) +
           (s.center.z - nearest.z) * (s.center.z - nearest.z) <= s.radius * s.radius;
}

const bool CollisionChecker::SphereToRay(const Primitive::Sphere& s, const Primitive::Ray& r, float* dist, Vector3* intersection)
{
    Vector3 m = Vector3(r.start - s.center).Normalize();
    float b = Math::Vec3::Dot(m, r.dir);
    float c = Math::Vec3::Dot(m, m) - s.radius * s.radius;

    if (c > 0.f && b > 0.f) { return false; }

    float discr = b * b - c;
    if (discr < 0.f) { return false; }

    float t = -b - std::sqrtf(discr);
    if (t < 0.f) { t = 0.f; }
    if (dist) { *dist = t; }

    if (intersection) { *intersection = r.start + t * r.dir; }

    return true;
}

const bool CollisionChecker::PlaneToPlane(const Primitive::Plane& p1, const Primitive::Plane& p2)
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

const bool CollisionChecker::AABBToAABB(const Primitive::AABB& a1, const Primitive::AABB& a2)
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

const bool CollisionChecker::AABBToPoint(const Primitive::AABB& a, const Primitive::Point& p)
{
    return (a.center.x - a.radius.x <= p.pos.x && p.pos.x <= a.center.x + a.radius.x) &&
           (a.center.y - a.radius.y <= p.pos.y && p.pos.y <= a.center.y + a.radius.y) &&
           (a.center.z - a.radius.z <= p.pos.z && p.pos.z <= a.center.z + a.radius.z);
}

const bool CollisionChecker::OBBToPoint(const Primitive::OBB& o, const Primitive::Point& p)
{
    Matrix4 obbMat = { o.orientations[0].x,o.orientations[0].y,o.orientations[0].z,0,
                       o.orientations[1].x,o.orientations[1].y,o.orientations[1].z,0,
                       o.orientations[2].x,o.orientations[2].y,o.orientations[2].z,0,
                       o.center.x,o.center.y,o.center.z,1 };

    Matrix4 obbMatInverse = Math::Mat4::Inverse(obbMat);
    Vector3 obbLocal_pointPos = Math::Mat4::Transform(p.pos, obbMatInverse);

    Primitive::AABB obbLocal_aabb;
    obbLocal_aabb.center = { 0,0,0 };
    obbLocal_aabb.radius = o.radius;

    return (obbLocal_aabb.center.x - obbLocal_aabb.radius.x <= obbLocal_pointPos.x && obbLocal_pointPos.x <= obbLocal_aabb.center.x + obbLocal_aabb.radius.x) &&
           (obbLocal_aabb.center.y - obbLocal_aabb.radius.y <= obbLocal_pointPos.y && obbLocal_pointPos.y <= obbLocal_aabb.center.y + obbLocal_aabb.radius.y) &&
           (obbLocal_aabb.center.z - obbLocal_aabb.radius.z <= obbLocal_pointPos.z && obbLocal_pointPos.z <= obbLocal_aabb.center.z + obbLocal_aabb.radius.z);
}

const bool CollisionChecker::PointToPoint(const Primitive::Point& p1, const Primitive::Point& p2)
{
    return p1.pos == p2.pos;
}
