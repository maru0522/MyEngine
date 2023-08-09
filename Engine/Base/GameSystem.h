#pragma once
#include "FrameWork.h"

class GameSystem final :
    public FrameWork
{
public:
    // �֐�
    GameSystem(void) {};

    void Initialize() override;
    void Update(void) override;
    void Draw(void) override;
    void Finalize(void) override;
};
