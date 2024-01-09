#pragma once
#include "Sound.h"
#include "Planet.h"
#include "Object3D.h"
#include "LightManager.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"

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

    //>> 関数
    Coin(CollisionManager* arg_colMPtr);
    ~Coin(void) override;

    void Initialize(const Axis3& arg_posture, const Transform& arg_transform, bool arg_adaptPosture = false, bool arg_rePop = false);
    void Update(void) override;
    void Draw(void) override;

private:
    void Collision_onTrigger(void);
    void Collision_onCollision(void);

    //>> 変数
    Axis3 axes_;
    float radian_;         // どんくらい回転してるか
    Vector3 vec3_newUp_;   // 新規上ベクトル
    bool is_adaptPosture_; // 姿勢を惑星から見た位置に自動で適応するか。

    Transform transform_;  // 座標等
    bool is_taken_{};      // 回収されたか
    bool is_rePop_{};      // リポップするか

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
    void SetupCircleShadows(Planet* arg_planetPtr, LightManager* arg_lightManagerPtr, const Vector3& arg_atten = { 0.02f,0.06f,0.01f }, const Vector2& arg_factorAngle = { 6.f,8.f }, float arg_distAtCaster = 1.f);
    void SetCircleShadowActive(LightManager* arg_lightManagerPtr, bool arg_isActive) { arg_lightManagerPtr->SetLightActive(LightType::CIRCLE_SHADOW, circleShadows_num_, arg_isActive); }
    void SetCircleShadowDistAtCaster(LightManager* arg_lightManagerPtr, float arg_dist) { arg_lightManagerPtr->SetLightDistanceAtCaster(LightType::CIRCLE_SHADOW, circleShadows_num_, arg_dist); }

    // getter
    const Vector3& GetPosition(void) { return transform_.position; }
    Transform* GetTransformPtr(void) { return &transform_; }
    bool GetIsTaken(void) { return is_taken_; }
};

