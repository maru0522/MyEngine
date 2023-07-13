#pragma once
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include <functional>

namespace CollisionPrimitive
{
    struct SphereCollider;
    struct PlaneCollider;
}

struct CollisionInfo
{
    CollisionInfo(void) noexcept = default;
    CollisionInfo(const Vector3 v, float f,const std::string& id) noexcept : v(v), f(f), id(id) {}

    void Reset(void)
    {
        v = { 0.f,0.f,0.f };
        f = 0.f;
        id.assign("Reseted");
    }

    std::string id{};
    Vector3 v{};
    float f{};
};

class ICollider
{
public:
    // ä÷êî
    ICollider(void) = default;
    virtual ~ICollider(void) = default;
    virtual bool Dispatch(ICollider* other) = 0;

    virtual bool Col(CollisionPrimitive::SphereCollider* arg_Shpere) = 0;
    virtual bool Col(CollisionPrimitive::PlaneCollider* arg_Plane) = 0;

protected:
    // ïœêî
    std::string id_{};
    CollisionInfo colInfo_{};
    std::function<void(void)> onCollision_{};

public:
    // setter
    inline void SetID(const std::string& id) { id_ = id; }
    inline void SetColInfo(const CollisionInfo& colInfo) { colInfo_ = colInfo; }
    inline void SetOnCollision(const std::function<void(void)> callback_onCollision) { onCollision_ = callback_onCollision; }

    // getter
    inline const std::string& GetID(void) { return id_; }
    inline const CollisionInfo& GetColInfo(void) { return colInfo_; }
    inline const std::function<void(void)>& GetOnCollision(void) { return onCollision_; }

};