#pragma once
#include <string>
#include "Vector2.h"
#include "Vector3.h"

namespace CollisionPrimitive
{
    struct SphereCollider;
    struct PlaneCollider;
}

struct CollisionInfo
{
    CollisionInfo(void) noexcept = default;
    CollisionInfo(const Vector3 v, float f) noexcept : v(v), f(f) {}

    void Reset(void)
    {
        v = { 0.f,0.f,0.f };
        f = 0.f;
    }

    Vector3 v{};
    float f{};
};

class ICollider
{
public:
    // ä÷êî
    ICollider(void) : isHit_(false) {}
    virtual ~ICollider(void) = default;
    virtual bool Dispatch(ICollider* other) = 0;

    virtual bool Col(const CollisionPrimitive::SphereCollider* arg_Shpere) = 0;
    virtual bool Col(const CollisionPrimitive::PlaneCollider* arg_Plane) = 0;

protected:
    // ïœêî
    bool isHit_;
    CollisionInfo colInfo_;

public:
    // setter
    inline void SetIsHit(bool isHit) { isHit_ = isHit; }

    // getter
    inline bool GetIsHit(void) { return isHit_; }
    inline const CollisionInfo& GetColInfo(void) { return colInfo_; }

};