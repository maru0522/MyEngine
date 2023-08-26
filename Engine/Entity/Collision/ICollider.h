#pragma once
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include <functional>

namespace CollisionPrimitive
{
    struct SphereCollider;
    struct PlaneCollider;
    struct PointCollider;
    struct AABBCollider;

    enum class Shape
    {
        UNKNOWN, // 未定義
        SPHERE,  // 球
        PLANE,   // 平面
        POINT,   // 点
        AABB,    // 直方体
    };
}

class ICollider
{
public:
    // 関数
    ICollider(void) = default;
    virtual ~ICollider(void) = default;
    virtual bool Dispatch(ICollider* other) = 0;

    virtual bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) = 0;
    virtual bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) = 0;
    virtual bool Col(CollisionPrimitive::PointCollider* arg_Point) = 0;
    virtual bool Col(CollisionPrimitive::AABBCollider* arg_AABB) = 0;

protected:
    void AllOnFunction(void) // なぜか上手くいかない 処理が通らない(?)せいで正常に動かない
    {
        if (onCollision_) { onCollision_; }
        if (onTrigger_) { onTrigger_; }
        if (onRelease_) { onRelease_; }
    }

    // 変数
    std::string id_{};
    ICollider* other_{};
    CollisionPrimitive::Shape shape_{}; // 条件分岐時に、形状単位だと便利かも

    std::function<void(void)> onCollision_{};
    std::function<void(void)> onTrigger_{};
    std::function<void(void)> onRelease_{};

public:
    // setter
    void SetID(const std::string& id) { id_ = id; }
    void SetOther(ICollider* col) { other_ = col; }
    void SetOnCollision(const std::function<void(void)> callback_onCollision) { onCollision_ = callback_onCollision; }
    void SetOnTrigger(const std::function<void(void)> callback_onTrigger) { onTrigger_ = callback_onTrigger; }
    //inline void SetOnCollision(const std::function<void(void)> callback_onRelease) { onRelease_ = callback_onRelease; }

    // getter
    const std::string& GetID(void) { return id_; }
    ICollider* GetOther(void) { return other_; }
    CollisionPrimitive::Shape GetShape(void) { return shape_; }
    const std::function<void(void)>& GetOnCollision(void) { return onCollision_; }
    const std::function<void(void)>& GetOnTrigger(void) { return onTrigger_; }
    const std::function<void(void)>& GetOnRelease(void) { return onRelease_; }

};