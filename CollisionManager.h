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

    // ��`
    using Shape = ColliderFactory::Shape;

    // �ϐ�
    ColliderFactory colFactory_;
    std::forward_list<ICollider*> collidersPtrList_;

public:
    // �֐�
    static CollisionManager* GetInstance(void);

    void Update(void);

    // setter
    inline void Register(ICollider* colliderPtr) { collidersPtrList_.push_front(colliderPtr); }
    inline void Remove(ICollider* colliderPtr) { collidersPtrList_.remove(colliderPtr); }
};

