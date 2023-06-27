#pragma once
#include "FrameWork.h"
#include "PostEffect.h"
#include "GaussianBlur.h"

class GameSystem :
    public FrameWork
{
public:
    // �֐�
    GameSystem(void) {};

    void Initialize() override;
    void Update(void) override;
    void Draw(void) override;
    void Finalize(void) override;

private:
    // �ϐ�
    std::unique_ptr<PostEffect> postEffect_;
};
