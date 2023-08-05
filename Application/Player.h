#pragma once
#include <memory>
#include "Object3D.h"
#include "CameraManager.h"
#include "CollisionPrimitive.h"

class Player
{
public:
    // �萔
    const float kRadius_{ 1.f };
    const float kJumpPower_{ 7.f };
    const float kMoveSpeed_{ 1.f };
    const float kGravity_{ 0.98f };

    // �֐�
    Player(CameraManager* camMPtr);

    void Update(void);
    void Draw(void);

private:
    void Move(Vector3& moveVec,Vector3& velocity); // get velocity & moveVec
    void OnCollision(void); // callback

    // �ϐ�
    //bool isGrounded_{};
    WorldCoordinate2 coordinate_;
    CollisionPrimitive::SphereCollider sphereCollider_;
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/lolP/lolP.obj") };

    float jumpVecNorm_{};
    CameraManager* camMPtr_{};
public:
    // setter

    // getter
    inline WorldCoordinate2* GetCoordinatePtr(void) { return &coordinate_; }
    inline const CollisionPrimitive::SphereCollider& GetSphereCollider(void) { return sphereCollider_; }
    inline const std::string& GetPartnerId(void) { return sphereCollider_.GetColInfo().id; }
};

