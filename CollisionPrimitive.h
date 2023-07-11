#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "ICollider.h"

namespace CollisionPrimitive
{
    struct SphereCollider final : public ICollider
    {
        // 関数
        SphereCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // 変数
        Vector3 center{ 0, 0, 0 };
        float radius{ 0.f };

    private:
        bool Col(const CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(const CollisionPrimitive::PlaneCollider* arg_Plane) override;
    };

    struct PlaneCollider final : public ICollider
    {
    public:
        // 関数
        PlaneCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // 変数
        Vector3 normal{ 0, 1, 0 };
        float distance{ 0.f };
    private:
        bool Col(const CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(const CollisionPrimitive::SphereCollider* arg_Shpere) override;
    };
}