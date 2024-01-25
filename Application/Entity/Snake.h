#pragma once
#include "Transform.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
#include <memory>
#include "Object3D.h"
#include "Planet.h"
#include "LightManager.h"
#include "ExclamationMark.h"
#include "Timer.h"
#include "SnakeBehavior.h"
#include "SnakeCommonInfomation.h"

/**
 * @file Snake.h
 * @brief 蛇クラス
 */

class Snake
{
private:
    //>> 定義
    friend ISnakeBehavior;

    enum class CorrectionDirection // ジャンプする際に左右どちらにぶれるのか表す列挙型
    {
        RIGHT,
        LEFT,
    };

public:
    // 関数
    Snake(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr);
    ~Snake(void);

    void Update(void);
    void Draw(void);

private:
    //void Move(Vector3& moveVec, Vector3& velocity); // get velocity & moveVec
    // プレイヤーを上ベクトルを軸に回転させる。※上ベクトルは変化しない
    void RotateDirection(float arg_radian);
    void Move(void);
    void Process_CircleShadow(void);

    void OnCollision(void); // callback
    void OnDetect(void); // callback

    //>> 変数
    bool isCaptured_{};

    CorrectionDirection shakeDirection_; // 移動する際に、左右どちらにぶれるか


    CollisionManager* colMPtr_;
    CollisionPrimitive::SphereCollider sphere_collision_;
    CollisionPrimitive::SphereCollider sphere_detect_;

    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/snake/snake.obj") };
    std::unique_ptr<ExclamationMark> exclamationMark_;
    DeltaTimer timer_visibleExclamationMark_; //

    LightManager* lightManagerPtr_;
    int32_t circleShadows_num_;
    Planet* planetPtr_;

    std::shared_ptr<SnakeCommonInfomation> commonInfo_;
    SnakeBehaviorMachine snakeBehaviorMachine_;

public:
    //>> setter
    void SetIsCaptured(bool arg_isCaptured) { isCaptured_ = arg_isCaptured; }
    void SetupLightCircleShadows(void) {
        // 使用可能な丸影の番号を取得
        circleShadows_num_ = lightManagerPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);
        lightManagerPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true);
    }
    void SetCircleShadowsIsActive(bool arg_active) { lightManagerPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, arg_active); }

    //>> getter
    Transform* GetTransformPtr(void) { return &commonInfo_->transform_; }
    bool GetIsCaptured(void) { return isCaptured_; }
};

