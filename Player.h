#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Player
{
public:
    // ä÷êî
    Player(void);

    void Update(void);
    void Draw(void);

    // ïœêî
    Vector3 forwardVec_{ 0,0,1 };
    Vector3 rightVec_{ 1,0,0 };
    Vector3 upVec_{ 0,1,0 };

    Vector3 velocity_;
    CollisionPrimitive::Sphere sphereCollider_;
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};

