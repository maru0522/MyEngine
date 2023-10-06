#pragma once
#include "Object3D.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

class Rock :
    public Object3D
{
public:
    // 定数
    const Vector3 kRadius_{ 10,1,1 };

    // 関数
    Rock(CollisionManager* colMPtr);
    ~Rock(void);

    void Update(void) override;
    void Draw(void) override;

private:
    void OnCollision(void);

    // 変数
    Transform transform_;
    Axis3 axes_;

    CollisionPrimitive::AABBCollider collision_aabb_;
    CollisionManager* colMPtr_;

public:
    // getter
    Transform* TransformPtr(void) { return &transform_; }
};

