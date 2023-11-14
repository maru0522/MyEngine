#pragma once
#include "CollisionPrimitive.h"

/*
 * 球
 * 点
 * 平面
 * XYZ軸に平行な直方体（AABB)
 * 分離軸に平行な直方体（OBB)
 * 半直線
 */

namespace CollisionChecker
{
    // 球と球
    const bool SphereToSphere(const CollisionPrimitive::SphereCollider& s1, const CollisionPrimitive::SphereCollider& s2);
    // 球と点
    const bool SphereToPoint(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PointCollider& p);
    // 球と平面
    const bool SphereToPlane(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PlaneCollider& p, Vector3* intersection = nullptr);
    // 球とAABB
    const bool SphereToAABB(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::AABBCollider& a, Vector3* intersection = nullptr);
    // 球とOBB
    const bool SphereToOBB(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::OBBCollider& o, Vector3* intersection = nullptr);
    // 球と線
    const bool SphereToRay(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::RayCollider& r,float* dist = nullptr, Vector3* intersection = nullptr);

    // 平面と平面
    const bool PlaneToPlane(const CollisionPrimitive::PlaneCollider& p1, const CollisionPrimitive::PlaneCollider& p2);
    // 平面と点
    const bool PlaneToPoint(const CollisionPrimitive::PlaneCollider& pl, const CollisionPrimitive::PointCollider& po);
    // 平面とAABB
    const bool PlaneToAABB(const CollisionPrimitive::PlaneCollider& p, const CollisionPrimitive::AABBCollider& a);
    // 平面とOBB
    const bool PlaneToOBB(const CollisionPrimitive::PlaneCollider& p, const CollisionPrimitive::AABBCollider& a);
    // 平面と線
    const bool PlaneToRay(const CollisionPrimitive::PlaneCollider& p, const CollisionPrimitive::RayCollider& r);

    // AABBとAABB
    const bool AABBToAABB(const CollisionPrimitive::AABBCollider& a1, const CollisionPrimitive::AABBCollider& a2);
    // AABBとOBB
    const bool AABBToOBB(const CollisionPrimitive::AABBCollider& a, const CollisionPrimitive::OBBCollider& o);
    // AABBと点
    const bool AABBToPoint(const CollisionPrimitive::AABBCollider& a, const CollisionPrimitive::PointCollider& p);
    // AABBと線
    const bool AABBToRay(const CollisionPrimitive::AABBCollider& a, const CollisionPrimitive::RayCollider& r);

    // OBBとOBB
    const bool OBBToOBB(const CollisionPrimitive::OBBCollider& o1, const CollisionPrimitive::OBBCollider& o2);
    // OBBと点
    const bool OBBToPoint(const CollisionPrimitive::OBBCollider& o, const CollisionPrimitive::PointCollider& p);
    // OBBと線
    const bool OBBToRay(const CollisionPrimitive::OBBCollider& o, const CollisionPrimitive::RayCollider& r);

    // 線と線
    const bool RayToRay(const CollisionPrimitive::RayCollider& r1, const CollisionPrimitive::RayCollider& r2);
    // 線と点
    const bool RayToPoint(const CollisionPrimitive::RayCollider& r, const CollisionPrimitive::PointCollider& p);

    // 点と点
    const bool PointToPoint(const CollisionPrimitive::PointCollider& p1, const CollisionPrimitive::PointCollider& p2);
}
