#pragma once
#include "Object3D.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
/**
 * @file Pipe.h
 * @brief 土管クラス
 */

class Pipe final : public Object3D
{
public:
    //>> 関数
    Pipe(CollisionManager* colMPtr);
    ~Pipe(void) override;

    void Update(void) override;
    void Draw(void) override;

private:
    void Collision_PushBack(void);
    void Collision_EnterInside(void);

    //>> 変数
    Transform transform_;
    Axis3 axes_;

    CollisionManager* colMPtr_{};
    CollisionPrimitive::SphereCollider collision_pushback_; // 当たり判定
    CollisionPrimitive::SphereCollider collision_enterInside_; // 土管に入る時の当たり判定

    Pipe* partnerPipePtr_{};
public:
    //>> setter
    void SetPosition(const Vector3& arg_pos);
    void SetRotation(const Vector3& arg_rot) { transform_.rotation = arg_rot; }
    void SetRadius_ColPushBack(float arg_radius) { collision_pushback_.radius = arg_radius; }
    void SetRadius_ColEnterInside(float arg_radius) { collision_enterInside_.radius = arg_radius; }
    void SetPartnerPtr(Pipe* arg_partnerPtr) { partnerPipePtr_ = arg_partnerPtr; }

    // getter
    const Vector3& GetPosition(void) { return transform_.position; }
    Transform* GetTransformPtr(void) { return &transform_; }
    CollisionPrimitive::SphereCollider* GetColPushbackPtr(void) { return &collision_pushback_; }
};

