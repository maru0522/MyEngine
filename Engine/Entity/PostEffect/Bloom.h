#pragma once
#include "PostEffect.h"
#include "HighLumi.h"
class Bloom :
    public PostEffect/*, public HighLumi*/
{
public:
    Bloom(void);
    ~Bloom(void) override = default;

    void Initialize(void) override;
    void PreDrawScene(void) override;
    void PostDrawScene(void) override;
    void Draw(void) override;
};

