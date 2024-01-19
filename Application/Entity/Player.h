#pragma once
#include <memory>
#include "Object3D.h"
#include "CameraManager.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
#include "Planet.h"
#include "LightManager.h"
#include "PlayerBehavior.h"
#include "PlayerUI.h"
#include "PlayerCommonInfomation.h"
#include "OperateGuideUI.h"

/**
 * @file Player.h
 * @brief Playerクラスのヘッダー
 */

class Player
{
public:
    //>> 定義
    friend IPlayerBehavior;

    //>> 関数
    Player(CameraManager* arg_camMPtr, CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr);
    ~Player(void);

    void Update(void);
    void Draw3d(void);
    void Draw2dFore(void);

    Vector3 pos_backDiagonalAbove_; // プレイヤーの背後斜め上の点
    Vector3 pos_withinRangeAtBGAPoint_; // プレイヤーの斜め上の点から、カメラが離れられる最大範囲のpos
    float dist_maxRangeAtBGAPoint_; // プレイヤーの斜め上の点から、カメラが離れられる最大距離

    int32_t captureCount_rabbit; // 兎を捕まえた回数

    // イベントでカメラを動かし終えた後、追従カメラに遷移する際に、その肝心の追従カメラがプレイヤーの上にいなかったら困るので、
    // きちんとプレイヤーの上にいさせる為に座標を渡す関数。
    void HandOverToBehindCamera(const std::string& arg_camId);
private:
    void ControlUI(void);

    // callback
    void OnCollision(void);
    void OnTrigger(void);

    //>> 変数
    std::shared_ptr<Player_CommonInfomation> commonInfo_;

    CollisionManager* colMPtr_;
    CollisionPrimitive::SphereCollider sphereCollider_;
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/sphere/sphere.obj") };
    //std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/character/chr_sword.obj") };
    //std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/chicken/chicken.obj") };
    //std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/player/Casual_Male.obj") };

    PlayerBehaviorMachine pbm_;
    PlayerUI playerUI_;
    OperateGuideUI operateGuideUI_;

    int32_t circleShadows_num_;
    Planet* planetPtr_;
    LightManager* lightManagerPtr_;

public:
    //>> setter
    void SetupLightCircleShadows(void) {
        // 使用可能な丸影の番号を取得
        circleShadows_num_ = lightManagerPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);
        lightManagerPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true);
    }
    void SetAxes(const Axis3& arg_axes) { commonInfo_->axes_ = arg_axes; }
    void SetPosition(const Vector3& arg_pos) { commonInfo_->transform_.position = arg_pos; }
    void SetEventState(PlayerEventState arg_eventState) { commonInfo_->eventState_ = arg_eventState; }

    //>> getter
    TransformMatrix* GetTransMatPtr(void) { return &commonInfo_->matTrans_; }
    Transform* GetTransformPtr(void) { return &commonInfo_->transform_; }
    const Transform GetTransform(void) { return commonInfo_->transform_; }
    Axis3* GetAxis3Ptr(void) { return &commonInfo_->axes_; }
    const CollisionPrimitive::SphereCollider& GetSphereCollider(void) { return sphereCollider_; }
    const std::string& GetPartnerId(void) { return sphereCollider_.GetOther()->GetID(); }

    PlayerBehaviorMachine* GetPBM(void) { return &pbm_; }
};

