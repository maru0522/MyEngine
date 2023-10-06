#pragma once
#include "PostEffect.h"
class HighLumi :
    public PostEffect
{
public:
    HighLumi(void);

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};
