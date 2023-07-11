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
    std::forward_list<ICollider*> collidersPtrList_; // いったん途中。コライダームズすんぎ。

public:
    // 関数
    static CollisionManager* GetInstance(void);

    void Update(void);

    // setter
    //inline void AddColList(Shape shape) { collidersList_.emplace_back(std::move(colFactory_.CreateCollider(shape))); }

    // getter
    //inline ICollider* GetListBackColPtr(void) { return collidersList_.back().get(); }
    //inline std::list<std::unique_ptr<ICollider>>* GetColListPtr(void) { return &collidersList_; }
};

