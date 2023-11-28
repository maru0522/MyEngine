#pragma once
#include "FrameWork.h"
#include "BackGroundTexture.h"

class GameSystem final :
    public FrameWork
{
public:
    // 関数
    GameSystem(void) {};

    void Initialize() override;
    void Update(void) override;
    void Draw(void) override;
    void Finalize(void) override;
};
