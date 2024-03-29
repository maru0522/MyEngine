#pragma once
#include "PostEffect.h"
class HighLumi :
    public PostEffect
{
public:
    HighLumi(void);
    ~HighLumi(void) override = default;

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};
