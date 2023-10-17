#pragma once
#include <cstdint>
#include "Timer.h"

class ISceneTransition
{
public:
    //>> 関数
    ISceneTransition(void) noexcept = default;
    virtual ~ISceneTransition(void) = default;

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

class SceneTransition_FadeIn final : public ISceneTransition 
{
public:
    //>> 関数
    SceneTransition_FadeIn(void) = default;
    ~SceneTransition_FadeIn(void) override = default;

    void Execute(void) override;
    void Display(void) override;

    void Start(void) override { timer_.Start(100); }
    void Finish(void) override { timer_.Finish(); }
};