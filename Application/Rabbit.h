#pragma once
#include "WorldCoordinate.h"
#include "Transform.h"
#include "CollisionPrimitive.h"
#include <memory>
#include "Object3D.h"

class Rabbit
{
public:
    const float kRadius_{ 1.f };
    const float kDetectRadius_{ 10.f };
    const float kJumpPower_{ 7.f };
    const float kMoveSpeed_{ 1.f };
    const float kGravity_{ 0.98f };

    // ä÷êî
    Rabbit(void);

    void Update(void);
    void Draw(void);

private:
    void Move(Vector3& moveVec, Vector3& velocity); // get velocity & moveVec

    void OnCollision(void); // callback
    void OnDetectPlayer(void); // callback

    // ïœêî
    Vector3 pPos_;

    Coordinate coordinate_;
    Transform transform_;
    Axis3 axes_;

    CollisionPrimitive::SphereCollider sphereCollider_;
    CollisionPrimitive::SphereCollider detectPlayerCollider_;

    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/lolP/lolP.obj") };

    float jumpVecNorm_{};
};

