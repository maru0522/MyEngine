#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Player
{
public:
    // íËêî
    const float kRadius_{ 1.f };
    const float kJumpPower_{ 5.f };
    //const float kJumpPower_{ 7.f };
    const float kMoveSpeed_{ 1.f };
    const float kGravity_{ 0.4f };
    //const float kGravity_{ 0.98f };

    // ä÷êî
    Player(void);

    void Update(void);
    void Draw(void);

private:
    void OnCollision(void);

    // ïœêî
    //bool isGrounded_{};
    WorldCoordinate coordinate_;
    CollisionPrimitive::SphereCollider sphereCollider_;
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };

    Vector3 jumpVec_;
public:
    // setter

    // getter
    inline WorldCoordinate* GetCoordinatePtr(void) { return &coordinate_; }
    inline const CollisionPrimitive::SphereCollider& GetSphereCollider(void) { return sphereCollider_; }
    inline const std::string& GetPartnerId(void) { return sphereCollider_.GetColInfo().id; }
};

