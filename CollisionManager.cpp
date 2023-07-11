#include "CollisionManager.h"
#include "CollisionChecker.h"

CollisionManager* CollisionManager::GetInstance(void)
{
    static CollisionManager ins;
    return &ins;
}

void CollisionManager::Update(void)
{
    //auto it1 = collidersPtrList_.begin();
    //for (; it1 != collidersPtrList_.end(); ++it1)
    //{
    //    // it1 ‚ÌŽŸ
    //    auto it2 = it1;
    //    it2++;

    //    for (; it2 != collidersPtrList_.end(); ++it2)
    //    {
    //        it1->
    //        it1->Dispatch(&(*it2));
    //    }
    //}
    //for (auto& col1 : collidersPtrList_)
    //{
    //    for (auto& col2 : collidersPtrList_)
    //    {
    //        if (col1 == col2) continue;


    //    }
    //}
}
