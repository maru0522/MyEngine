#pragma once
#include "UI.h"

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
    bool is_end_{};

public:
    //>> setter
    void SetUIPtr(UI* arg_uiPtr) { uiPtr_ = arg_uiPtr; }
    void SetIsVisible(bool arg_isVisible) { is_visible_ = arg_isVisible; }
};