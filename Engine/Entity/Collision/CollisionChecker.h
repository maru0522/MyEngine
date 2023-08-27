#pragma once
#include "CollisionPrimitive.h"

/*
 * 球
 * 点
 * 平面
 * 直方体
 */

namespace CollisionChecker
{
    // 球と球
    const bool SphereToSphere(const CollisionPrimitive::SphereCollider& s1, const CollisionPrimitive::SphereCollider& s2);
    // 球と点
    const bool SphereToPoint(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PointCollider& p);
    // 球と平面
    const bool SphereToPlane(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PlaneCollider& p, Vector3* intersection = nullptr);
    // 球と直方体
    const bool SphereToAABB(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::AABBCollider& a, Vector3* intersection = nullptr);

    // 平面と平面
    const bool PlaneToPlane(const CollisionPrimitive::PlaneCollider& p1, const CollisionPrimitive::PlaneCollider& p2);
    // 平面と点
    const bool PlaneToPoint(const CollisionPrimitive::PlaneCollider& pl, const CollisionPrimitive::PointCollider& po);
    // 平面と直方体
    const bool PlaneToAABB(const CollisionPrimitive::PlaneCollider& p, const CollisionPrimitive::PlaneCollider& a);

    // 直方体と直方体
    const bool AABBToAABB(const CollisionPrimitive::AABBCollider& a1, const CollisionPrimitive::AABBCollider& a2);
    // 直方体と点
    const bool AABBToPoint(const CollisionPrimitive::AABBCollider& a, const CollisionPrimitive::PointCollider& p);

    // 点と点
    const bool PointToPoint(const CollisionPrimitive::PointCollider& p1, const CollisionPrimitive::PointCollider& p2);
}