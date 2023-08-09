#pragma once
#include "IScene.h"
class TitleScene final :
    public IScene
{
public:
    // �֐�
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;
};

