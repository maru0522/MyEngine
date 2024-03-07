#pragma once
#include "UI.h"
#include "Timer.h"
/**
 * @file Player_UI.h
 * @brief プレイヤー用のUIに関する処理を纏めたクラスが宣言されたファイル
 */

class PlayerUI
{
private:
    //>> 定義
    const float kDefPos_right_ = 1230.f;
    const float kDefPos_left_ = -80.f;
    const float kMoveDist_ = 115.f;

public:
    //>> 関数
    PlayerUI(void) = default;
    PlayerUI(UI* arg_uiPtr) : uiPtr_(arg_uiPtr) {};
    ~PlayerUI(void);

    void Initialize(void);
    void Update(void);
    void Draw(void);

private:
    void EaseUI(void);

    //>> 変数
    UI* uiPtr_;
    bool is_visible_{};
    bool is_visiblePre_{};
    int32_t* rabbitCount_{};
    int32_t* coinCount_{};

    DeltaTimer easeTimer_;

    bool IsTrigger(void) { return is_visible_ && is_visiblePre_ == false; }
    bool IsExecute(void) { return is_visible_; }
    bool IsRelease(void) { return is_visible_ == false && is_visiblePre_; }

public:
    //>> setter
    void SetUIPtr(UI* arg_uiPtr) { uiPtr_ = arg_uiPtr; }
    void SetIsVisible(bool arg_isVisible) { is_visible_ = arg_isVisible; }
    void SetRabbitCountPtr(int32_t* arg_rabbitCountPtr) { rabbitCount_ = arg_rabbitCountPtr; }
    void SetCoinCountPtr(int32_t* arg_coinCountPtr) { coinCount_ = arg_coinCountPtr; }

    //>> getter
    bool GetIsEnd(void) { return easeTimer_.GetIsExecute(); }
};