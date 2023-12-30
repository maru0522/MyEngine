#pragma once
#include "PostEffect.h"
/**
 * @file HighLumi.h
 * @brief ポストエフェクトのHighLumiについてのクラスが宣言されたファイル
 */

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
