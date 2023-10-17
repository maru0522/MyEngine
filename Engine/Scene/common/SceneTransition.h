#pragma once
#include <cstdint>
#include "Timer.h"
#include "Sprite.h"

class ISceneTransition
{
public:
    //>> 関数
    ISceneTransition(void) noexcept = default;
    virtual ~ISceneTransition(void) = default;

    virtual void Initialize(void) = 0;
    virtual void Execute(void) = 0;
    virtual void Display(void) = 0;

    virtual void Start(void) = 0;
    virtual void Finish(void) = 0;
protected:
    //>> 変数
    FrameTimer timer_;

public:
    //>> getter
    FrameTimer& GetTimer(void) { return timer_; }
};

class SceneTransition_FadeInOut final : public ISceneTransition 
{
public:
    //>> 関数
    SceneTransition_FadeInOut(void) = default;
    ~SceneTransition_FadeInOut(void) override = default;

    void Initialize(void) override;
    void Execute(void) override;
    void Display(void) override;

    void Start(void) override;
    void Finish(void) override { timer_.Finish(); }

private:
    //>> 変数
    Sprite png_deepBlue_{ "Resources/BackGround.png" };
    bool is_out_; // 暗転しきったか
};