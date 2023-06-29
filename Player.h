#pragma once
#include <memory>
#include "Object3D.h"

class Player
{
public:
    // �֐�
    Player(void);

    void Update(void);
    void Draw(void);

private:
    // �ϐ�
    std::unique_ptr<Object3D> body_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
};

