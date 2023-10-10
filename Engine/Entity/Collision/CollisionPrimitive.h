#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "ICollider.h"

namespace CollisionPrimitive
{
    struct PointCollider final : public ICollider
    {
        // 関数
        PointCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // 変数
        Vector3 pos{ 0, 0, 0 };

    private: //4
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
        bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
    };

    struct SphereCollider final : public ICollider
    {
        // 関数
        SphereCollider(void) 
        {
            shape_ = Shape::SPHERE;
        }
        SphereCollider(const std::string& id)
        {
            id_ = id;
            shape_ = Shape::SPHERE;
        }

        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // 変数
        Vector3 center{ 0, 0, 0 };
        float radius{ 0.f };

    private:
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
        bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
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

    private://2
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
        bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
    };

    struct AABBCollider final : public ICollider
    {
    public:
        // 関数
        AABBCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // 変数
        Vector3 center{ 0, 0, 0 };
        Vector3 radius{ 0, 0, 0 };
        Vector3 inter{ 0, 0, 0 };

    private://2
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
        bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
    };

    struct OBBCollider final :public ICollider
    {
    public:
        // 関数
        OBBCollider(void) noexcept = default;
        inline bool Dispatch(ICollider* other) override { return other->Col(this); }

        // 変数
        Vector3 center{ 0, 0, 0 };
        Vector3 orientations[3]{ {},{},{} };
        Vector3 radius{ 0, 0, 0 };
        Vector3 inter{ 0, 0, 0 };

    private://2
        bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) override;
        bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) override;
        bool Col(CollisionPrimitive::PointCollider* arg_Point) override;
        bool Col(CollisionPrimitive::AABBCollider* arg_AABB) override;
        bool Col(CollisionPrimitive::OBBCollider* arg_OBB) override;
    };
}
