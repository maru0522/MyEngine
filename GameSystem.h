#pragma once
#include "FrameWork.h"
class GameSystem :
    public FrameWork
{
public:
    // ä÷êî
    GameSystem(void) {};

    void Initialize() override;
    void Update(void) override;
    void Draw(void) override;
    void Finalize(void) override;
};

