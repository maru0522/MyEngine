#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Planet
{
public:
    // 定数
    const float kScale_{ 20.f };
    const float kGravityArea_{ 50.f };

    // 関数
    Planet(void);

    void Update(void);
    void Draw(void);

    CollisionPrimitive::SphereCollider surface_;
    CollisionPrimitive::SphereCollider gravityArea_;

private:
    void OnCollision(void) {}

    // 変数
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};
