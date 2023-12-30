#pragma once
#include "CollisionPrimitive.h"
// -------------------------------------------
/**
 * @file CollisionChecker.h
 * @brief 各矩形同士の当たり判定用関数がひととおり宣言されたファイル
 */
// -------------------------------------------


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
    const bool SphereToSphere(const Primitive::Sphere& s1, const Primitive::Sphere& s2);
    // 球と点AABBCollider
    const bool SphereToPoint(const Primitive::Sphere& s, const Primitive::Point& p);
    // 球と平面
    const bool SphereToPlane(const Primitive::Sphere& s, const Primitive::Plane& p, Vector3* intersection = nullptr);
    // 球とAABB
    const bool SphereToAABB(const Primitive::Sphere& s, const Primitive::AABB& a, Vector3* intersection = nullptr);
    // 球とOBB
    const bool SphereToOBB(const Primitive::Sphere& s, const Primitive::OBB& o, Vector3* intersection = nullptr);
    // 球と線
    const bool SphereToRay(const Primitive::Sphere& s, const Primitive::Ray& r,float* dist = nullptr, Vector3* intersection = nullptr);

    // 平面と平面
    const bool PlaneToPlane(const Primitive::Plane& p1, const Primitive::Plane& p2);
    // 平面と点
    const bool PlaneToPoint(const Primitive::Plane& pl, const Primitive::Point& po);
    // 平面とAABB
    const bool PlaneToAABB(const Primitive::Plane& p, const Primitive::AABB& a);
    // 平面とOBB
    const bool PlaneToOBB(const Primitive::Plane& p, const Primitive::AABB& a);
    // 平面と線
    const bool PlaneToRay(const Primitive::Plane& p, const Primitive::Ray& r);

    // AABBとAABB
    const bool AABBToAABB(const Primitive::AABB& a1, const Primitive::AABB& a2);
    // AABBとOBB
    const bool AABBToOBB(const Primitive::AABB& a, const Primitive::OBB& o);
    // AABBと点
    const bool AABBToPoint(const Primitive::AABB& a, const Primitive::Point& p);
    // AABBと線
    const bool AABBToRay(const Primitive::AABB& a, const Primitive::Ray& r);

    // OBBとOBB
    const bool OBBToOBB(const Primitive::OBB& o1, const Primitive::OBB& o2);
    // OBBと点
    const bool OBBToPoint(const Primitive::OBB& o, const Primitive::Point& p);
    // OBBと線
    const bool OBBToRay(const Primitive::OBB& o, const Primitive::Ray& r);

    // 線と線
    const bool RayToRay(const Primitive::Ray& r1, const Primitive::Ray& r2);
    // 線と点
    const bool RayToPoint(const Primitive::Ray& r, const Primitive::Point& p);

    // 点と点
    const bool PointToPoint(const Primitive::Point& p1, const Primitive::Point& p2);
}
