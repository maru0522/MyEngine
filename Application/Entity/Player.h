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

class Player
{
public:
    //>> 定義
    friend IPlayerBehavior;

    enum class EventState
    {
        NONE,
        PLANET_HOLE,
    };

    //>> 定数
    const float kRadius_{ 1.f };
    const float kJumpPower_{ 0.75f };
    const float kJumpLongPower_{ 1.3f };
    const float kMoveSpeed_{ 1.f };
    const float kMoveJumpLongSpeed_{ 2.f };
    const float kGravity_{ 0.03f };

    //>> 関数
    Player(CameraManager* arg_camMPtr, CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr);
    ~Player(void);

    void Update(void);
    void Draw3d(void);
    void Draw2dFore(void);

    float default_rad_{ 50.f };
    float current_rad_{ default_rad_ };

    Vector3 pos_backDiagonalAbove_; // プレイヤーの背後斜め上の点
    Vector3 pos_withinRangeAtBGAPoint_; // プレイヤーの斜め上の点から、カメラが離れられる最大範囲のpos
    float dist_maxRangeAtBGAPoint_; // プレイヤーの斜め上の点から、カメラが離れられる最大距離

    int32_t captureCount_rabbit; // 兎を捕まえた回数

private:
    void ControlUI(void);

    // callback
    void OnCollision(void);
    void OnTrigger(void);

    //>> 変数
    TransformMatrix matTrans_;
    Transform transform_;
    Axis3 axes_;

    Vector3 moveVec_;
    Vector3 velocity_; // めり込んだ時に押し戻せるように
    float jumpVecNorm_{};
    bool isLanding_{};

    int32_t coinNum_;

    CollisionManager* colMPtr_;
    CollisionPrimitive::SphereCollider sphereCollider_;
    //std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/sphere/sphere.obj") };
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/character/chr_sword.obj") };
    //std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/player/Casual_Male.obj") };

    CameraManager* camMPtr_{};

    PlayerBehaviorMachine pbm_;
    PlayerUI playerUI_;

    int32_t circleShadows_num_;
    Planet* planetPtr_;
    LightManager* lightManagerPtr_;

    EventState eventState_;
    
    Vector2 vec2_direction_ = { 0,1 };
    // プレイヤーが初期姿勢: Axis3({0,0,1},{1,0,0},{0,1,0}) ※(正面,右,上)から、
    // 上ベクトルを軸として何度回転したか、(正面と右を回転させる）
    float rot_playerVector_;
public:
    //>> setter
    void SetupLightCircleShadows(void) {
        // 使用可能な丸影の番号を取得
        circleShadows_num_ = lightManagerPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);
        lightManagerPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true);
    }
    void SetPosition(const Vector3& arg_pos) { transform_.position = arg_pos; }
    void SetEventState(EventState arg_eventState) { eventState_ = arg_eventState; }

    //>> getter
    TransformMatrix* GetTransMatPtr(void) { return &matTrans_; }
    Transform* GetTransformPtr(void) { return &transform_; }
    const Transform GetTransform(void) { return transform_; }
    Axis3* GetAxis3Ptr(void) { return &axes_; }
    const CollisionPrimitive::SphereCollider& GetSphereCollider(void) { return sphereCollider_; }
    const std::string& GetPartnerId(void) { return sphereCollider_.GetOther()->GetID(); }

    PlayerBehaviorMachine* GetPBM(void) { return &pbm_; }
};

