#pragma once
#include "Transform.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
#include <memory>
#include "Object3D.h"
#include "Planet.h"
#include "LightManager.h"

class Rabbit
{
public:
    const float kRadius_{ 1.f };
    const float kDetectRadius_{ 10.f };
    const float kJumpPower_{ 7.f };
    const float kMoveSpeed_{ 0.2f };
    const float kGravity_{ 0.98f };

    // 関数
    Rabbit(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr);
    ~Rabbit(void);

    void Update(void);
    void Draw(void);

private:
    void Move(Vector3& moveVec, Vector3& velocity); // get velocity & moveVec

    void OnCollision(void); // callback
    void OnDetectPlayer(void); // callback

    // 変数
    Vector3 pPos_;
    bool isCaptured_{};

    TransformMatrix coordinate_;
    Transform transform_;
    Axis3 axes_;

    CollisionManager* colMPtr_;
    CollisionPrimitive::SphereCollider sphereCollider_;
    CollisionPrimitive::SphereCollider detectPlayerCollider_;

    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/rabbit/rabbit.obj") };

    float jumpVecNorm_{};
    int32_t circleShadows_num_;
    Planet* planetPtr_;
    LightManager* lightManagerPtr_;

public:
    // setter
    void SetIsCaptured(bool arg_isCaptured) { isCaptured_ = arg_isCaptured; }
    void SetupLightCircleShadows(void) {
        // 使用可能な丸影の番号を取得
        circleShadows_num_ = lightManagerPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);
        lightManagerPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true);
    }

    // getter
    Transform* GetTransformPtr(void) { return &transform_; }
    bool GetIsCaptured(void) { return isCaptured_; }
};

