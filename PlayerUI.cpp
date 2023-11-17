#include "PlayerUI.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

PlayerUI::~PlayerUI(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->UnRegister("playerUI_starCount");
    uiPtr_->UnRegister("playerUI_coinCount");
    uiPtr_->UnRegister("playerUI_starDustCount");
    uiPtr_->UnRegister("playerUI_remain");
    uiPtr_->UnRegister("playerUI_life");
}

void PlayerUI::Initialize(void)
{
    is_visible_ = false;

    easeTimer_.Start(0.f);
    easeTimer_.SetAddSpeed(2.f);
    easeTimer_.Update();

    // nullチェック
    if (!uiPtr_) { return; }

    // 星の数の画像
    uiPtr_->Register("playerUI_starCount", "Resources/starCount.png");
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ -80.f ,40.f });
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetAlpha(0.f);

    // コインの数の画像
    uiPtr_->Register("playerUI_coinCount", "Resources/coinCount.png");
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ 1195.f, 580.f });
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAlpha(0.f);

    // 星屑の数の画像
    uiPtr_->Register("playerUI_starDustCount", "Resources/starDustCount.png");
    uiPtr_->GetUISpritePtr("playerUI_starDustCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_starDustCount")->SetPosition(Vector2{ 1080.f, 640.f });

    // 残機の数の画像
    uiPtr_->Register("playerUI_remain", "Resources/remain.png");
    uiPtr_->GetUISpritePtr("playerUI_remain")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_remain")->SetPosition(Vector2{ -80.f ,640.f });
    uiPtr_->GetUISpritePtr("playerUI_remain")->SetAlpha(0.f);

    // HPの画像
    uiPtr_->Register("playerUI_life", "Resources/life.png");
    uiPtr_->GetUISpritePtr("playerUI_life")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ 1060.f + 115.f, 50.f });
    uiPtr_->GetUISpritePtr("playerUI_life")->SetAlpha(0.f);
}

void PlayerUI::Update(void)
{
    // imgui
    //GUI::Begin("PlayerUI");
    //GUI::CheckBox("is_visible_", &is_visible_);
    //GUI::Text("rate:  %f", easeTimer_.GetTimeRate());
    //GUI::Text("frame: %f", easeTimer_.GetFrameCurrent());
    //GUI::End();

    EaseUI();

    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Update("playerUI_starCount");
    uiPtr_->Update("playerUI_coinCount");
    uiPtr_->Update("playerUI_starDustCount");
    uiPtr_->Update("playerUI_remain");
    uiPtr_->Update("playerUI_life");

    // フラグ更新
    is_visiblePre_ = is_visible_;
}

void PlayerUI::Draw(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Draw("playerUI_starCount");
    uiPtr_->Draw("playerUI_coinCount");
    uiPtr_->Draw("playerUI_starDustCount");
    uiPtr_->Draw("playerUI_remain");
    uiPtr_->Draw("playerUI_life");
}

void PlayerUI::EaseUI(void)
{
    if (IsTrigger() || IsRelease())
    {
        easeTimer_.Start(0.6f);
        easeTimer_.SetAddSpeed(2.f);
    }

    if (is_visible_)
    {
        easeTimer_.Update();

        // 星の数の画像
        const Vector2& starCount_defPos = { -80.f, 40.f };
        const float starCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), starCount_defPos.x, starCount_defPos.x + 115.f);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ starCount_posX,starCount_defPos.y });
        const float starCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetAlpha(starCount_alpha);

        // コインの数の画像
        const Vector2& coinCount_defPos = { 1195.f, 580.f };
        const float coinCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), coinCount_defPos.x, coinCount_defPos.x - 115.f);
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ coinCount_posX,coinCount_defPos.y });
        const float coinCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAlpha(coinCount_alpha);

        // 残機の数の画像
        const Vector2& remain_defPos = { -80.f ,640.f };
        const float remain_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), remain_defPos.x, remain_defPos.x + 115.f);
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetPosition(Vector2{ remain_posX,remain_defPos.y });
        const float remain_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetAlpha(remain_alpha);

        // HPの画像
        const Vector2& life_defPos = { 1175.f, 50.f };
        const float life_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), life_defPos.x, life_defPos.x - 115.f);
        uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ life_posX,life_defPos.y });
        const float life_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_life")->SetAlpha(life_alpha);
    }
    else
    {
        easeTimer_.Update();

        // 星の数の画像
        const Vector2& starCount_defPos = { 35.f, 40.f };
        const float starCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), starCount_defPos.x, starCount_defPos.x - 115.f);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ starCount_posX,starCount_defPos.y });
        const float starCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(),1.f,0.f);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetAlpha(starCount_alpha);

        // コインの数の画像
        const Vector2& coinCount_defPos = { 1080.f, 580.f };
        const float coinCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), coinCount_defPos.x, coinCount_defPos.x + 115.f);
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ coinCount_posX,coinCount_defPos.y });
        const float coinCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAlpha(coinCount_alpha);

        // 残機の数の画像
        const Vector2& remain_defPos = { 35.f ,640.f };
        const float remain_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), remain_defPos.x, remain_defPos.x - 115.f);
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetPosition(Vector2{ remain_posX,remain_defPos.y });
        const float remain_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetAlpha(remain_alpha);

        // HPの画像
        const Vector2& life_defPos = { 1060.f, 50.f };
        const float life_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), life_defPos.x, life_defPos.x + 115.f);
        uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ life_posX,life_defPos.y });
        const float life_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_life")->SetAlpha(life_alpha);
    }
}
