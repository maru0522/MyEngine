#pragma once
#include "Object3D.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
#include "Sound.h"
#include "Planet.h"
#include "LightManager.h"

class Coin :
    public Object3D
{
public:
    Coin(CollisionManager* arg_colMPtr);
    ~Coin(void) override;

    void Update(void) override;
    void Draw(void) override;

private:
    void Collision_onTrigger(void);

    //>> 変数
    Transform transform_;
    Axis3 axes_;

    bool is_taken_{};

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

