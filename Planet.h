#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Planet
{
public:
    // �֐�
    Planet(void);

    void Update(void);
    void Draw(void);

    CollisionPrimitive::SphereCollider sphereCollider_;

private:
    // �ϐ�
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};

