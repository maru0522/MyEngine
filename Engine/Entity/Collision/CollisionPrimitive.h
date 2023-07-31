#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "ICollider.h"

namespace CollisionPrimitive
{
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
    private:
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
    };
}