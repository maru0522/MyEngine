#pragma once
#include "SceneTransition.h"

class SceneTransition_FadeIn final : public ISceneTransition
{
public:
    //>> 関数
    SceneTransition_FadeIn(void);
    ~SceneTransition_FadeIn(void) override = default;

    void Initialize(void) override;
    void Execute(void) override;
    void Display(void) override;

    void Start() override;
    void Finish(void) override { timer_.Finish(); }

private:
    //>> 変数
    Sprite png_white1x1_{ "Resources/white1x1.png" };
};