#pragma once
#include "Object3D.h"
#include "CollisionManager.h"

class StickyBall :
    public Object3D
{
public:
    // 定数
    const float kDistance_to_root_{ 1.f };      // 根本から浮いてる球までの距離
    const float kDistance_to_defPos_{ 5.f };    // 元の球の座標からどれだけ離れることができるか
    const float kRadius_{ 2.f };                // 球自体の大きさ

    // 関数
    StickyBall(CollisionManager* colMPtr);
    ~StickyBall(void);

    void Update(void) override;
    void Draw(void) override;

private:
    void OnCollision(void);

    // 変数
    Transform transform_; // 根元の座標
    Axis3 axes_;

    CollisionPrimitive::SphereCollider collision_sphere_; // 球の座標
    CollisionManager* colMPtr_;
    float distance_to_defPos_;

public:
    // getter
    Transform* TransformPtr(void) { return &transform_; }

    // setter
    void SetRoot(const Vector3& pos);
};

