#include "CollisionManager.h"
#include "Collision.h"

CollisionManager* CollisionManager::GetInstance(void)
{
    static CollisionManager ins;
    return &ins;
}

void CollisionManager::Update(void)
{
    //for (auto& sphere : sphColPtrList_)
    //{
    //    sphere->isHit = Collision::SphereToSphere(sphere)
    //}
}
