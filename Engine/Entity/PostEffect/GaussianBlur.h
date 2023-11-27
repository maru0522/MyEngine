#pragma once
#include "PostEffect.h"
class GaussianBlur :
    public PostEffect
{
public:
    GaussianBlur(void);
    ~GaussianBlur(void) override = default;

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};
