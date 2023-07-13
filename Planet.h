#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Planet
{
public:
    // íËêî
    const float kScale_{ 20.f };
    const float kGravityArea_{ 50.f };

    // ä÷êî
    Planet(void);

    void Update(void);
    void Draw(void);

    CollisionPrimitive::SphereCollider surface_;
    CollisionPrimitive::SphereCollider gravityArea_;

private:
    void OnCollision(void) {}

    // ïœêî
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};
