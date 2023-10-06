#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Planet
{
public:
    // 定数
    const float kScale_{ 50.f };
    const float kGravityArea_{ 500.f };
    const float kRepelCameraArea_{ 100.f };

    // 関数
    Planet(void);
    ~Planet(void);

    void Update(void);
    void Draw(void);

    Transform transform_;

    CollisionPrimitive::SphereCollider surface_;
    CollisionPrimitive::SphereCollider gravityArea_;
    CollisionPrimitive::SphereCollider repelCameraArea_;

private:
    void OnCollision(void) {}

    // 変数
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/sphere/sphere.obj") };

public:
    // getter
    const Vector3& GetPosition(void) { return transform_.position; }
};
