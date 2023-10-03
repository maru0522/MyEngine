#pragma once
#include <memory>
#include "Object3D.h"
#include "CameraManager.h"
#include "CollisionPrimitive.h"
#include "PlayerBehavior.h"

class Player
{
public:
    //>> 定義
    friend IPlayerBehavior;

    //>> 定数
    const float kRadius_{ 1.f };
    const float kJumpPower_{ 3.f };
    const float kMoveSpeed_{ 1.f };
    const float kGravity_{ 0.2f };

    //>> 関数
    Player(CameraManager* camMPtr);
    ~Player(void);

    void Update(void);
    void Draw(void);

    float default_rad_{ 65.f };
    float current_rad_{ default_rad_ };
    float theta_{ 0.85f };
    float phi_;

    int32_t captureCount_rabbit; // 兎を捕まえた回数
    bool isFallHole1_;
    bool isFallHole2_;
private:
    void Move(Vector3& moveVec, Vector3& velocity); // get velocity & moveVec
    void SphericalCamera(Vector3& inputVec); // correct value for camera
    void OnCollision(void); // callback

    //>> 変数
    TransformMatrix coordinate_;
    Transform transform_;
    Axis3 axes_;

    Vector3 moveVec_;
    Vector3 velocity_; // めり込んだ時に押し戻せるように

    CollisionPrimitive::SphereCollider sphereCollider_;
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/lolP/lolP.obj") };

    float jumpVecNorm_{};
    CameraManager* camMPtr_{};

    PlayerBehaviorMachine pbm_;
public:
    //>> setter

    //>> getter
    inline TransformMatrix* GetTransMatPtr(void) { return &coordinate_; }
    Transform* GetTransformPtr(void) { return &transform_; }
    Axis3* GetAxis3Ptr(void) { return &axes_; }
    inline const CollisionPrimitive::SphereCollider& GetSphereCollider(void) { return sphereCollider_; }
    inline const std::string& GetPartnerId(void) { return sphereCollider_.GetOther()->GetID(); }
};

