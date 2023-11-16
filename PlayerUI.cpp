#include "PlayerUI.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

PlayerUI::~PlayerUI(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->UnRegister("playerUI_starCount");
    uiPtr_->UnRegister("playerUI_coinCount");
    uiPtr_->UnRegister("playerUI_remain");
    uiPtr_->UnRegister("playerUI_life");
}

void PlayerUI::Initialize(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Register("playerUI_starCount", "Resources/starCount.png");
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ 35.f,40.f });

    uiPtr_->Register("playerUI_coinCount", "Resources/coinCount.png");
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAnchorPoint(Vector2{ 0.f,0.f });
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ 1100.f, 600.f });

    uiPtr_->Register("playerUI_remain", "Resources/remain.png");

    uiPtr_->Register("playerUI_life", "Resources/life.png");
    uiPtr_->GetUISpritePtr("playerUI_life")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ 1060.f, 50.f });
}

void PlayerUI::Update(void)
{
    // フラグ更新
    //is_visiblePre_ = is_visible_;

    EaseUI();

    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Update("playerUI_starCount");
    uiPtr_->Update("playerUI_coinCount");
    uiPtr_->Update("playerUI_remain");
    uiPtr_->Update("playerUI_life");

    GUI::Begin("PlayerUI");
    GUI::CheckBox("is_visible_", &is_visible_);
    GUI::Text("rate:  %f", easeTimer_.GetTimeRate());
    GUI::Text("frame: %f", easeTimer_.GetFrameCurrent());
    GUI::End();
}

void PlayerUI::Draw(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Draw("playerUI_starCount");
    uiPtr_->Draw("playerUI_coinCount");
    //uiPtr_->Draw("playerUI_remain");
    uiPtr_->Draw("playerUI_life");
}

void PlayerUI::EaseUI(void)
{
    //if(IsTrigger() && is_end_)
    //{
    //    is_end_ = false;

    //    easeTimer_.Start(1.f);
    //}

    //if (IsExecute() == false) { return; }

    if (is_visible_ && is_visiblePre_ == false)
    {
        is_visiblePre_ = true;
        easeTimer_.Start(3.f);
    }

    if (is_visible_)
    {
        easeTimer_.Update();

        const Vector2& defPos_starCount = { 35.f,40.f };
        const float posX_starCount = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), defPos_starCount.x, defPos_starCount.x + 100.f);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ posX_starCount,defPos_starCount.y });
    }
}
