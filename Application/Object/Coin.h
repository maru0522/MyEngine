#pragma once
#include "Object3D.h"
#include "CollisionPrimitive.h"
#include "CollisionManager.h"
#include "Sound.h"

class Coin :
    public Object3D
{
public:
    Coin(CollisionManager* colMPtr);
    ~Coin(void) override;

    void Update(void) override;
    void Draw(void) override;

private:
    void Collision_Contact(void);

    //>> 変数
    Transform transform_;
    Axis3 axes_;

    bool is_taken_{};
    bool is_takenPre_{};

    CollisionManager* colMPtr_{};
    CollisionPrimitive::SphereCollider collision_contact_; // 当たり判定

    Sound se_getCoin_{ "Resources/sound/coin.wav" };

public:
    //>> setter
    void SetPosition(const Vector3& arg_pos);
    void SetRotation(const Vector3& arg_rot) { transform_.rotation = arg_rot; }
    void SetRadius_Contact(float arg_radius) { collision_contact_.radius = arg_radius; }

    // getter
    const Vector3& GetPosition(void) { return transform_.position; }
    Transform* GetTransformPtr(void) { return &transform_; }
};

