#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "ICollider.h"

namespace CollisionPrimitive
{
    struct PointCollider final : public ICollider
    {
        // �֐�
        PointCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // �ϐ�
        Vector3 pos{ 0, 0, 0 };

    private: //4
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
    };

    struct SphereCollider final : public ICollider
    {
        // �֐�
        SphereCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // �ϐ�
        Vector3 center{ 0, 0, 0 };
        float radius{ 0.f };

    private:
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
    };

    struct PlaneCollider final : public ICollider
    {
    public:
        // �֐�
        PlaneCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // �ϐ�
        Vector3 normal{ 0, 1, 0 };
        float distance{ 0.f };

    private://2
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
    };

    struct AABBCollider final : public ICollider
    {
    public:
        // �֐�
        AABBCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // �ϐ�
        Vector3 center{ 0, 0, 0 };
        Vector3 radius{ 0, 0, 0 };

    private://2
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
    };
}