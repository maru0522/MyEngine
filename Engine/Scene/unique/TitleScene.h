#pragma once
#include "IScene.h"
#include "Sprite.h"
#include "ICamera.h"
#include "DebugCamera.h"
#include "Timer.h"

class TitleScene final :
    public IScene
{
public:
    //>> 関数
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;

private:
    //>> 変数
    std::unique_ptr<ICamera> camera_title{ std::make_unique<ICamera>("titleScene") };
    DeltaTimer dTimer_ease_logoUpScale_;
    DeltaTimer dTimer_ease_WhitelogoDownScale_;
    DeltaTimer dTimer_ease_pressAStrAlpha_;
};

