#pragma once
#include "UI.h"
#include "Timer.h"
#include "CameraManager.h"
/**
 * @file OperateGuideUI.h
 * @brief プレイヤー用のUIに関する処理を纏めたクラスが宣言されたファイル
 */

class OperateGuideUI
{
private:
    //>> 定義
    const float kTimer_visivleEase_ = 5.f;   // 何秒かけて描画イージングするか
    const float kTimer_invisivleEase_ = 5.f; // 何秒かけて透明イージングするか
    const float kTimer_noOperate_ = 10.f;    // 何秒放置してたらガイドが出るか
    const float kMoveDistance = 60.f;        // イージング時どれくらい移動するか

public:
    //>> 関数
    OperateGuideUI(void) = default;
    OperateGuideUI(UI* arg_uiPtr,CameraManager* arg_camMPtr);
    ~OperateGuideUI(void);

    void Initialize(void);
    void Update(const Matrix4& arg_playerMatW);
    void Draw(void);

private:
    void EaseUI(const Matrix4& arg_playerMatW);

    bool IsVisibleTrigger(void) { return is_visible_pre_ == false && is_visible_; }
    bool IsVisibleDown(void) { return is_visible_; }
    bool IsVisibleRelease(void) { return is_visible_pre_ && is_visible_ == false; }

    //>> 変数
    UI* uiPtr_;
    CameraManager* camMPtr_;

    bool is_visible_{};
    bool is_visible_pre_{};
    DeltaTimer timer_visibleEase_;
    DeltaTimer timer_invisibleEase_;
    DeltaTimer timer_noOperate_;

public:
    //>> setter
    void SetUIPtr(UI* arg_uiPtr) { uiPtr_ = arg_uiPtr; }
    void SetCamMPtr(CameraManager* arg_camMPtr) { camMPtr_ = arg_camMPtr; }

    //>> getter
    //bool GetIsEnd(void) { return easeTimer_.GetIsExecute(); }
};