#pragma once
#include <list>
#include "ICollider.h"
#include "CollisionPrimitive.h"

class CollisionManager
{
public:
    // ä÷êî
    static CollisionManager* GetInstance(void);

    void Update(void);
private:
    //std::list<Collider<CollisionPrimitive::Sphere>> sphereColliderList_;
    std::list<CollisionPrimitive::Sphere*> sphColPtrList_;

    // singleton
    CollisionManager(void) {};
    ~CollisionManager(void) {};
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;
};

