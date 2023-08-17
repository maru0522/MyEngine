#pragma once
#include <memory>
#include "Object3D.h"
#include "CameraManager.h"
#include "CollisionPrimitive.h"

class Player
{
public:
    // íËêî
    const float kRadius_{ 1.f };
    const float kJumpPower_{ 7.f };
    const float kMoveSpeed_{ 1.f };
    const float kGravity_{ 0.98f };

    // ä÷êî
    Player(CameraManager* camMPtr);

    void Update(void);
    void Draw(void);

    float default_rad_{ 60.f };
    float current_rad_{ default_rad_ };
    float theta_{ 0.85f };
    float phi_;
private:
    void Move(Vector3& moveVec, Vector3& velocity); // get velocity & moveVec
    void SphericalCamera(Vector3& inputVec); // correct value for camera
    void OnCollision(void); // callback

    // ïœêî
    Coordinate coordinate_;
    Transform transform_;
    Axis3 axes_;

    CollisionPrimitive::SphereCollider sphereCollider_;
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/lolP/lolP.obj") };

    float jumpVecNorm_{};
    CameraManager* camMPtr_{};
public:
    // setter

    // getter
    inline Coordinate* GetCoordinatePtr(void) { return &coordinate_; }
    Transform* GetTransformPtr(void) { return &transform_; }
    Axis3* GetAxis3Ptr(void) { return &axes_; }
    inline const CollisionPrimitive::SphereCollider& GetSphereCollider(void) { return sphereCollider_; }
    inline const std::string& GetPartnerId(void) { return sphereCollider_.GetColInfo().id; }
};

