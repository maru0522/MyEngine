#include "CollisionManager.h"
#include "CollisionChecker.h"

CollisionManager* CollisionManager::GetInstance(void)
{
    static CollisionManager ins;
    return &ins;
}

void CollisionManager::Update(void)
{
    auto it1 = collidersPtrList_.begin();
    for (; it1 != collidersPtrList_.end(); ++it1)
    {
        // it1 の次
        auto it2 = it1;
        it2++;

        for (; it2 != collidersPtrList_.end(); ++it2)
        {
            // 衝突検出とcallback実行
            (*it1)->Dispatch(*it2);

            // 接触した瞬間の検出とcallback実行
            (*it1)->Check_onTrigger();
            (*it2)->Check_onTrigger();
            // 離れた瞬間の検出とcallback実行
            (*it1)->Check_onRelease();
            (*it2)->Check_onRelease();
        }

        // 衝突した相手の名前を1Frame前用のリストに記録
        (*it1)->RecordIds();
    }
}
