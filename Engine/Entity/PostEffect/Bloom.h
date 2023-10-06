#pragma once
#include "PostEffect.h"
#include "HighLumi.h"
class Bloom :
    public PostEffect/*, public HighLumi*/
{
public:
    Bloom(void);

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};

