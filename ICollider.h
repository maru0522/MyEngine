#pragma once
#include <string>

class ICollider
{
public:
    virtual ~ICollider(void) = default;
    virtual std::string GetTypeString(void) {};

    bool isHit_;
};

template<class CollisionPrimitive>
class Collider final:
    public ICollider
{
public:
    template<class CollisionPrimitive, class... Args>
    Collider(Args... collisionPrimitiveArgs) {
        info_ = CollisionPrimitive(collisionPrimitiveArgs);
    }
    virtual ~Collider(void) = default;

    virtual bool GetHit(void) = 0;

protected:
    CollisionPrimitive info_;
};