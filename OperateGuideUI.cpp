#include "OperateGuideUI.h"
#include "Input.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

OperateGuideUI::OperateGuideUI(UI* arg_uiPtr, CameraManager* arg_camMPtr)
    : uiPtr_(arg_uiPtr), camMPtr_(arg_camMPtr)
{
}

OperateGuideUI::~OperateGuideUI(void)
{
    // タイマーの初期化
    timer_visibleEase_.Finish(true);
    timer_invisibleEase_.Finish(true);
    timer_noOperate_.Finish(true);

    // nullチェック
    if (!uiPtr_) { return; }
    // Rスティックの画像
    uiPtr_->UnRegister("playerUI_operateGuide_RStick");
}

void OperateGuideUI::Initialize(void)
{
    // タイマーの初期化
    timer_visibleEase_.Finish(true);
    timer_invisibleEase_.Finish(true);
    timer_noOperate_.Finish(true);

    // nullチェック
    if (!uiPtr_) { return; }
    // Lスティックの画像
    uiPtr_->Register("playerUI_operateGuide_LStick", "Resources/LStick.png");
    uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetAlpha(0.f);
}

void OperateGuideUI::Update(const Matrix4& arg_playerMatW)
{
    // フラグの記録
    is_visible_pre_ = is_visible_;

    const bool isDownSomeKey = KEYS::IsSomeDown();
    //const bool isDownSomeButton = XPAD::IsSomeDown();
    const bool isOperateLStick = XPAD::GetLStick().IsNonZero();

    GUI::Begin("OperateGuideUI");
    GUI::Text("timer_noOp: [ %f / %f ] - (%f)", timer_noOperate_.GetFrameCurrent(), timer_noOperate_.GetFrameMax(), timer_noOperate_.GetTimeRate());
    GUI::Text(is_visible_ ? "is_visible_ : true" : "is_visible_ : false");
    GUI::Text(is_visible_pre_ ? "is_visible_pre_ : true" : "is_visible_pre_ : false");
    GUI::Text("timer_viea: [ %f / %f ] - (%f)", timer_visibleEase_.GetFrameCurrent(), timer_visibleEase_.GetFrameMax(), timer_visibleEase_.GetTimeRate());
    GUI::Text("pos: [%f,%f]", uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->GetPosition().x, uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->GetPosition().y);
    GUI::End();

    // 何も触っていない時
    if (isDownSomeKey == false /*&& isDownSomeButton == false*/ && isOperateLStick == false)
    {
        // 触れていなかった時間をカウントする
        timer_noOperate_.Update();
    }
    // どれかに一瞬でも触れたとき
    else
    {
        const float rate_invisible = timer_invisibleEase_.GetTimeRate();
        if (rate_invisible >= 1.f)
        {
            // 描画フラグfalse
            is_visible_ = false;

            // 初期化して起動
            timer_noOperate_.Finish(true);
            timer_noOperate_.Start(kTimer_noOperate_);
        }
    }


    const float rate_noOp = timer_noOperate_.GetTimeRate();
    // rateが1.f以上
    if (rate_noOp >= 1.f)
    {
        // 描画フラグtrue
        is_visible_ = true;
    }

    EaseUI(arg_playerMatW);
}

void OperateGuideUI::Draw(void)
{
    if (is_visible_ == false) { return; }
    uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->Draw();
}

void OperateGuideUI::EaseUI(const Matrix4& arg_playerMatW)
{
    // 取得したカメラがプレイヤー追従カメラでないならスキップ
    ICamera* icamPtr = camMPtr_->GetCurrentCamera();
    if (icamPtr->GetId().starts_with("BehindCamera_") == false) { return; }

    // ui用にプレイヤーの座標をスクリーン座標に変換
    const Vector2 playerScreenPos = icamPtr->GetScreen().WorldToScreenPoint(arg_playerMatW);

    // 起動時
    if (IsVisibleTrigger())
    {
        // 初期化して起動
        timer_visibleEase_.Finish(true);
        timer_visibleEase_.Start(kTimer_visivleEase_);
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetPosition(playerScreenPos);
    }
    // 実行中（ただし、起動時は除く）
    else if (IsVisibleDown())
    {
        // 更新処理
        timer_visibleEase_.Update();
        const float rate = timer_visibleEase_.GetTimeRate();

        // ui座標計算
        Vector2 pos;
        pos.y = -Math::Ease::EaseInOutSine(rate, 0.f, kMoveDistance); // スクリーン座標なのでy軸をマイナス
        pos += playerScreenPos;
        // ui座標設定
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetPosition(pos);

        // ui透明度イージング
        const float alpha = Math::Ease::EaseInOutSine(rate, 0.f, 0.7f);
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetAlpha(alpha);
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->Update();
    }
    // 終了時
    else if (IsVisibleRelease())
    {
        timer_invisibleEase_.Finish(true);
        timer_invisibleEase_.Start(kTimer_invisivleEase_);
    }
    // 描画しない状態の時
    else if (IsVisibleDown() == false)
    {
        // 更新処理
        timer_invisibleEase_.Update();
        const float rate = timer_invisibleEase_.GetTimeRate();

        // ui座標計算
        Vector2 pos;
        pos.y = Math::Ease::EaseInOutSine(rate, kMoveDistance, 0.f);
        pos += playerScreenPos; // スクリーン座標なのでy軸をマイナス
        // ui座標設定
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetPosition(pos);

        // ui透明度イージング
        const float alpha = Math::Ease::EaseInOutSine(rate, 0.7f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->SetAlpha(alpha);
        uiPtr_->GetUISpritePtr("playerUI_operateGuide_LStick")->Update();
    }
}
