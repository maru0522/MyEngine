#pragma once
#include "PostEffect.h"
class RadialBlur :
    public PostEffect
{
public:
    RadialBlur(void);

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};

