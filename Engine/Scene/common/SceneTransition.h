#pragma once
#include <cstdint>
#include "Timer.h"
#include "Sprite.h"
/**
 * @file SceneTransition.h
 * @brief シーン遷移クラスの基底となるクラスが宣言されたファイル
 */

enum class TransitionType
{
    FADEIN,
    MIDDLE,
    FADEOUT,
};

class ISceneTransition
{
public:
    //>> 関数
    ISceneTransition(TransitionType arg_transitionType) : transitionType_(arg_transitionType) {}
    virtual ~ISceneTransition(void) = default;

    virtual void Initialize(void) = 0;
    virtual void Execute(void) = 0;
    virtual void Display(void) = 0;

    virtual void Start(void) = 0;
    virtual void Finish(void) = 0;
protected:
    //>> 変数
    FrameTimer timer_;
    const TransitionType transitionType_;

public:
    //>> getter
    FrameTimer& GetTimer(void) { return timer_; }
};

//class SceneTransition_FadeInOut final : public ISceneTransition 
//{
//public:
//    //>> 関数
//    SceneTransition_FadeInOut(void) = default;
//    ~SceneTransition_FadeInOut(void) override = default;
//
//    void Initialize(void) override;
//    void Execute(void) override;
//    void Display(void) override;
//
//    void Start(void) override;
//    void Finish(void) override { timer_.Finish(); }
//
//private:
//    //>> 変数
//    Sprite png_deepBlue_{ "Resources/BackGround.png" };
//    bool is_out_; // 暗転しきったか
//};