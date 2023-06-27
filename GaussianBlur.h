#pragma once
#include "PostEffect.h"
class GaussianBlur :
    public PostEffect
{
public:
    GaussianBlur(void);

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};

