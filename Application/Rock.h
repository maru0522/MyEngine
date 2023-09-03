#pragma once
#include "Object3D.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

class Rock :
    public Object3D
{
public:
    // ’è”
    const Vector3 kRadius_{ 10,1,1 };

    // ŠÖ”
    Rock(CollisionManager* colMPtr);

    void Update(void) override;
    void Draw(void) override;

private:
    void OnCollision(void);

    // •Ï”
    Transform transform_;
    Axis3 axes_;

    CollisionPrimitive::AABBCollider collision_aabb_;

public:
    // getter
    Transform* TransformPtr(void) { return &transform_; }
};

