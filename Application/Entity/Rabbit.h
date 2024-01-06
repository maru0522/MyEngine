#pragma once
#include "Transform.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
#include <memory>
#include "Object3D.h"
#include "Planet.h"
#include "LightManager.h"
/**
 * @file Rabbit.h
 * @brief 兎クラス
 */

class Rabbit
{
private:
    //>> 定義
    enum class CorrectionDirection // ジャンプする際に左右どちらにぶれるのか表す列挙型
    {
        RIGHT,
        LEFT,
    };

public:
    const float kRadius_{ 1.f };                // 当たり判定の半径
    const float kDetectRadius_{ 40.f };         // プレイヤー検知の半径
    const float kDetectRadius_escape_{ 10.f };  // さらにこの半径内に入ったら、遠ざかる挙動
    const float kJumpPower_{ 0.4f };             // ジャンプ力
    const float kMoveSpeed_{ 0.38f };            // 移動速度
    const float kGravity_{ 0.025f };             // かかる重力値
    const float kMoveDist_{ 30.f };             // プレイヤーを検知した地点からどのくらいの距離移動するのか。

    // 関数
    Rabbit(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr);
    ~Rabbit(void);

    void Update(void);
    void Draw(void);

private:
    //void Move(Vector3& moveVec, Vector3& velocity); // get velocity & moveVec
    void Move(void);
    void Process_CircleShadow(void);

    void OnCollision(void); // callback
    void OnDetectPlayer(void); // callback

    //>> 変数
    bool isCaptured_{};


    TransformMatrix transformMatrix_; // 各ワールド行列
    Transform transform_;             // 座標等
    Axis3 axes_;                      // 姿勢
    Axis3 axes_old_;                  // 1F前の姿勢
    Vector3 vec3_newUp_;              // 新規姿勢の上ベクトル

    bool is_landing_;
    CorrectionDirection shakeDirection_; // 移動する際に、左右どちらにぶれるか
    Vector3 vec3_moveDirection_; // 移動する方向。
    float moveDist_; // 移動する距離。

    CollisionManager* colMPtr_;
    CollisionPrimitive::SphereCollider sphere_collision_;
    CollisionPrimitive::SphereCollider sphere_detectPlayer_;

    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/rabbit/rabbit.obj") };

    float velocity_vertical_{};
    int32_t circleShadows_num_;
    Planet* planetPtr_;
    LightManager* lightManagerPtr_;

public:
    //>> setter
    void SetIsCaptured(bool arg_isCaptured) { isCaptured_ = arg_isCaptured; }
    void SetupLightCircleShadows(void) {
        // 使用可能な丸影の番号を取得
        circleShadows_num_ = lightManagerPtr_->UsableRightNum(LightType::CIRCLE_SHADOW);
        lightManagerPtr_->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, true);
    }

    //>> getter
    Transform* GetTransformPtr(void) { return &transform_; }
    bool GetIsCaptured(void) { return isCaptured_; }
};

