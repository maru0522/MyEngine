#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Planet
{
public:
    // ä÷êî
    Planet(void);

    void Update(void);
    void Draw(void);

    CollisionPrimitive::SphereCollider sphereCollider_;

private:
    // ïœêî
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};

