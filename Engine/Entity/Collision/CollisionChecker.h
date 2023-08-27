#pragma once
#include "CollisionPrimitive.h"

/*
 * ��
 * �_
 * ����
 * ������
 */

namespace CollisionChecker
{
    // ���Ƌ�
    const bool SphereToSphere(const CollisionPrimitive::SphereCollider& s1, const CollisionPrimitive::SphereCollider& s2);
    // ���Ɠ_
    const bool SphereToPoint(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PointCollider& p);
    // ���ƕ���
    const bool SphereToPlane(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::PlaneCollider& p, Vector3* intersection = nullptr);
    // ���ƒ�����
    const bool SphereToAABB(const CollisionPrimitive::SphereCollider& s, const CollisionPrimitive::AABBCollider& a, Vector3* intersection = nullptr);

    // ���ʂƕ���
    const bool PlaneToPlane(const CollisionPrimitive::PlaneCollider& p1, const CollisionPrimitive::PlaneCollider& p2);
    // ���ʂƓ_
    const bool PlaneToPoint(const CollisionPrimitive::PlaneCollider& pl, const CollisionPrimitive::PointCollider& po);
    // ���ʂƒ�����
    const bool PlaneToAABB(const CollisionPrimitive::PlaneCollider& p, const CollisionPrimitive::PlaneCollider& a);

    // �����̂ƒ�����
    const bool AABBToAABB(const CollisionPrimitive::AABBCollider& a1, const CollisionPrimitive::AABBCollider& a2);
    // �����̂Ɠ_
    const bool AABBToPoint(const CollisionPrimitive::AABBCollider& a, const CollisionPrimitive::PointCollider& p);

    // �_�Ɠ_
    const bool PointToPoint(const CollisionPrimitive::PointCollider& p1, const CollisionPrimitive::PointCollider& p2);
}