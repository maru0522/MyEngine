#pragma once
#include "Sound.h"
#include "Planet.h"
#include "Object3D.h"
#include "LightManager.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"
#include "Timer.h"

/**
 * @file Coin.h
 * @brief コインクラス
 */

class Coin :
    public Object3D
{
public:
    //>> 定義
    const float kRadius_{ 1.3f };           // コインの半径
    const float kRotationRadian_{ 0.04f };  // コインを回転させるときの角速度（ラジアン）
    const float kRePopTime_{ 20.f };        // コインが復活するまでの時間（秒)
    const float kRePopScaleTime_{ 2.f };    // コインが何秒かけて元の大きさになるかの時間（秒)
    const Vector3 kScale_{ 1.f,1.f,1.f };   // コインの大きさ

    //>> 関数
    Coin(void);
    ~Coin(void) override;

    void SetUp(CollisionManager* arg_colMPtr, const std::string& arg_id, const Axis3& arg_posture, const Transform& arg_transform, bool arg_adaptPosture = false, bool arg_rePop = false);
    void Update(void) override;
    void Draw(void) override;

private:
    void Collision_onTrigger(void);
    void Collision_onCollision(void);

    //>> 変数
    std::string id_;
    Axis3 axes_;
    float radian_;                // どんくらい回転してるか
    Vector3 vec3_newUp_;          // 新規上ベクトル
    bool is_adaptPosture_;        // 姿勢を惑星から見た位置に自動で適応するか。

    Transform transform_;         // 座標等
    bool is_taken_{};             // 回収されたか
    bool is_rePop_{};             // リポップするか
    DeltaTimer timer_repop_;      // リポップするときの時間計測

    Vector2 factorAngleDefault_;  // factorAngle初期入力値
    bool is_startReScale_{};      // 再生成するときのフラグ
    DeltaTimer timer_repopScale_; // リポップするときのサイズ変更にかける時間

    CollisionManager* colMPtr_{};
    CollisionPrimitive::SphereCollider collision_contact_; // 当たり判定

    Sound se_getCoin_{ "Resources/sound/coin.wav" };

    int32_t circleShadows_num_;
    LightManager* lightManagerPtr_;

public:
    //>> setter
    void SetPosition(const Vector3& arg_pos);
    void SetRotation(const Vector3& arg_rot) { transform_.rotation = arg_rot; }
    void SetRadius_Contact(float arg_radius) { collision_contact_.radius = arg_radius; }
    void SetFactorAngleDefault(const Vector2& arg_factorAngleDefault) { factorAngleDefault_ = arg_factorAngleDefault; }
    void SetupCircleShadows(Planet* arg_planetPtr, LightManager* arg_lightManagerPtr, const Vector3& arg_atten = { 0.02f,0.06f,0.01f }, const Vector2& arg_factorAngle = { 6.f,8.f }, float arg_distAtCaster = 1.f);
    void SetCircleShadowActive(LightManager* arg_lightManagerPtr, bool arg_isActive) { arg_lightManagerPtr->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, arg_isActive); }
    void SetCircleShadowDistAtCaster(LightManager* arg_lightManagerPtr, float arg_dist) { arg_lightManagerPtr->SetLightDistanceAtCaster(LightType::CIRCLE_SHADOW, circleShadows_num_, arg_dist); }
    void SetCircleShadowFactorAngle(LightManager* arg_lightManagerPtr, const Vector2& arg_factorAngle) { arg_lightManagerPtr->SetLightFactorAngle(LightType::CIRCLE_SHADOW, circleShadows_num_, arg_factorAngle); }

    //>> getter
    const Vector3& GetPosition(void) { return transform_.position; }
    Transform* GetTransformPtr(void) { return &transform_; }
    bool GetIsTaken(void) { return is_taken_; }
    // is_taken == true && is_rePop == false の時、インスタンスごと消滅させる予定のため、その条件を満たしているか
    bool IsElimination(void) { return is_taken_ && is_rePop_ == false; }
};

