#pragma once
#include "SceneTransition.h"
class SceneTransition_Middle : public ISceneTransition
{
public:
    //>> 関数
    SceneTransition_Middle(void);
    ~SceneTransition_Middle(void) override = default;

    void Initialize(void) override;
    void Execute(void) override;
    void Display(void) override;

    void Start() override;
    void Finish(void) override { timer_.Finish(); }

private:
    //>> 変数
    Sprite png_white1x1_{ "Resources/white1x1.png" };
};

