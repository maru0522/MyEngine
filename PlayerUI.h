#pragma once
#include "UI.h"
#include "Timer.h"

class PlayerUI
{
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
    bool is_end_{};

    DeltaTimer easeTimer_;

    bool IsTrigger(void) { return is_visible_ && is_visiblePre_ == false; }
    bool IsExecute(void) { return is_visible_; }
    bool IsRelease(void) { return is_visible_ == false && is_visiblePre_; }

public:
    //>> setter
    void SetUIPtr(UI* arg_uiPtr) { uiPtr_ = arg_uiPtr; }
    void SetIsVisible(bool arg_isVisible) { is_visible_ = arg_isVisible; }
};