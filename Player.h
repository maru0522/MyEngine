#pragma once
#include <memory>
#include "Object3D.h"

class Player
{
public:
    // ŠÖ”
    Player(void);

    void Update(void);
    void Draw(void);

private:
    // •Ï”
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};

