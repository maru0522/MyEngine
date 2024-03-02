#pragma once
#include "Transform.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"
#include "Object3D.h"

class IPlanet
{
public:
    // 定数
    const float kScale_{ 50.f };
    const float kGravityArea_{ 500.f };
    const float kRepelCameraArea_{ 100.f };

    // 関数
    IPlanet(void) = default;
    virtual ~IPlanet(void) = default;

    virtual void Initialize(CollisionManager* arg_colMPtr);
    virtual void Update(void);
    virtual void Draw(void);
    virtual void Finalize(void);

    Transform transform_;

    CollisionManager* colMPtr_{};
    CollisionPrimitive::SphereCollider surface_;
    CollisionPrimitive::SphereCollider gravityArea_;
    bool isUV_;

protected:
    virtual void OnCollision(void) {}

    // 変数
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/sphere/sphere.obj") };

public:
    // getter
    const Vector3& GetPosition(void) { return transform_.position; }
};