#pragma once
#include <forward_list>
#include <memory>
#include "ColliderFactory.h"
#include "CollisionPrimitive.h"

class CollisionManager
{
private:
    // singleton
    CollisionManager(void) {};
    ~CollisionManager(void) {};
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    // 定義
    using Shape = ColliderFactory::Shape;

    // 変数
    ColliderFactory colFactory_;
    std::forward_list<ICollider*> collidersPtrList_;

public:
    // 関数
    static CollisionManager* GetInstance(void);

    void Update(void);

    // setter
    inline void Register(ICollider* colliderPtr) { collidersPtrList_.push_front(colliderPtr); }
    inline void Remove(ICollider* colliderPtr) { collidersPtrList_.remove(colliderPtr); }
};

