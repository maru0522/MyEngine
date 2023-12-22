#pragma once
#include "Transform.h"
#include "CameraManager.h"

enum class PlayerEventState
{
    NONE,
    PLANET_HOLE,
};

struct Player_CommonInfomation
{
    // 関数
    Player_CommonInfomation(void) = default;
    ~Player_CommonInfomation(void) = default;

    //>> 定数
    const float kRadius_{ 1.f };
    const float kJumpPower_{ 0.75f };
    const float kJumpLongPower_{ 0.85f };
    const float kMoveSpeed_{ 0.4f };
    const float kMoveJumpLongSpeed_{ 0.75f };
    const float kGravity_{ 0.03f };

    // 球面カメラの距離
    float default_rad_{ 50.f };
    float current_rad_{ default_rad_ };

    //>> 変数 
    TransformMatrix matTrans_;
    Transform transform_;

    // 姿勢
    Vector3 vec3_newUp_;
    Axis3 axes_;
    Axis3 axes_old_;
    // 進行方向に合わせた、モデル用の姿勢。
    Axis3 axes_4model_;

    // 入力
    Vector2 vec2_input_old_{ 0,1 };
    // 移動量
    Vector3 velocity_; // めり込んだ時に押し戻せるように
    // 
    float jumpVecNorm_{};
    bool isLanding_{};

    PlayerEventState eventState_{};

    CameraManager* camMPtr_{};
};

