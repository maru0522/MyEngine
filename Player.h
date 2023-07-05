#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Player
{
public:
    // ŠÖ”
    Player(void);

    void Update(void);
    void Draw(void);

    // •Ï”
    Vector3 forwardVec_;
    Vector3 rightVec_;
    Vector3 upVec_;


    Vector3 velocity_; // ˆÚ“®—Ê

    CollisionPrimitive::Sphere sphereCollider_;
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};

