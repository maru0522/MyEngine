#pragma once
#include "PostEffect.h"
class BackGroundTexture :
    public PostEffect
{
public:
    //>> 関数
    BackGroundTexture(void) = default;

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};

