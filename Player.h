#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"

class Player
{
public:
    // ’è”
    const float kRadius_{ 1.f };

    // ŠÖ”
    Player(void);

    void Update(void);
    void Draw(void);

private:
    // •Ï”
    WorldCoordinate coordinate_;
    CollisionPrimitive::Sphere sphereCollider_;
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };

public:
    // setter

    // getter
    inline WorldCoordinate* GetCoordinatePtr(void) { return &coordinate_; }
    inline const CollisionPrimitive::Sphere& GetSphereCollider(void) { return sphereCollider_; }

};

