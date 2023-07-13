#pragma once
#include <forward_list>
#include <memory>
#include "CollisionPrimitive.h"

class CollisionManager
{
private:
    // singleton
    CollisionManager(void) = default;
    ~CollisionManager(void) = default;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;

    // ��`
    std::forward_list<ICollider*> collidersPtrList_;

public:
    // �֐�
    static CollisionManager* GetInstance(void);

    void Update(void);

    // setter
    inline void Register(ICollider* colliderPtr) { collidersPtrList_.push_front(colliderPtr); }
    inline void UnRegister(ICollider* colliderPtr) { collidersPtrList_.remove(colliderPtr); }
};

