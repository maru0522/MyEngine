#pragma once
#include "IScene.h"
class GameScene :
    public IScene
{
public:
    //>> 関数
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;
};

