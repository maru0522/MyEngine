#include "PlayerUI.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

PlayerUI::~PlayerUI(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->UnRegister("playerUI_starCount");
    uiPtr_->UnRegister("playerUI_coinCount");
    uiPtr_->UnRegister("playerUI_coinFigures");
    uiPtr_->UnRegister("playerUI_starDustCount");
    uiPtr_->UnRegister("playerUI_remain");
    uiPtr_->UnRegister("playerUI_life");
    uiPtr_->UnRegister("playerUI_figures");
    uiPtr_->UnRegister("playerUI_figures2");
    uiPtr_->UnRegister("playerUI_slash");
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
    //uiPtr_->Register("playerUI_starCount", "Resources/RStick.png");
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetScale(Vector2{ 0.4f,0.4f });
    //uiPtr_->GetUISpritePtr("playerUI_starCount")->SetScale(Vector2{ 0.9f,0.9f });
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ kDefPos_left_ ,40.f });
    uiPtr_->GetUISpritePtr("playerUI_starCount")->SetAlpha(0.f);

    // コインの数の画像
    uiPtr_->Register("playerUI_coinCount", "Resources/coinCount.png");
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ kDefPos_left_, 580.f });
    uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAlpha(0.f);

    // コイン枚数の数字
    uiPtr_->Register("playerUI_coinFigures", "Resources/figures.png");
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetSize(Vector2{ 32.f,32.f });
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetPosition(Vector2{ kDefPos_left_, 580.f });
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetAlpha(0.f);
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetCutStartPoint(Vector2{ 0.f,0.f });
    uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetCutLength(Vector2{ 32.f,32.f });

    // 星屑の数の画像
    uiPtr_->Register("playerUI_starDustCount", "Resources/starDustCount.png");
    uiPtr_->GetUISpritePtr("playerUI_starDustCount")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_starDustCount")->SetPosition(Vector2{ 1115.f, 640.f });

    // 残機の数の画像
    uiPtr_->Register("playerUI_remain", "Resources/remain.png");
    uiPtr_->GetUISpritePtr("playerUI_remain")->SetScale(Vector2{ 0.4f,0.4f });
    uiPtr_->GetUISpritePtr("playerUI_remain")->SetPosition(Vector2{ kDefPos_left_ ,640.f });
    uiPtr_->GetUISpritePtr("playerUI_remain")->SetAlpha(0.f);

    // HPの画像
    uiPtr_->Register("playerUI_life", "Resources/life.png");
    uiPtr_->GetUISpritePtr("playerUI_life")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ kDefPos_right_, 50.f });
    uiPtr_->GetUISpritePtr("playerUI_life")->SetAlpha(0.f);

    // 数字
    uiPtr_->Register("playerUI_figures", "Resources/figures.png");
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetSize(Vector2{ 32.f,32.f });
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetPosition(Vector2{ WndAPI::kWidth_ / 2 - 30, 100.f });
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetAlpha(1.f);
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetCutStartPoint(Vector2{ 0.f,0.f });
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetCutLength(Vector2{ 32.f,32.f });

    // 数字2
    uiPtr_->Register("playerUI_figures2", "Resources/figures.png");
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetSize(Vector2{ 32.f,32.f });
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetPosition(Vector2{ WndAPI::kWidth_ / 2 + 30, 100.f });
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetAlpha(1.f);
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetCutStartPoint(Vector2{ 96.f,0.f });
    uiPtr_->GetUISpritePtr("playerUI_figures2")->SetCutLength(Vector2{ 32.f,32.f });

    // スラッシュ
    uiPtr_->Register("playerUI_slash", "Resources/slash.png");
    uiPtr_->GetUISpritePtr("playerUI_slash")->SetScale(Vector2{ 1.f,1.f });
    uiPtr_->GetUISpritePtr("playerUI_slash")->SetSize(Vector2{ 32.f,32.f });
    uiPtr_->GetUISpritePtr("playerUI_slash")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    uiPtr_->GetUISpritePtr("playerUI_slash")->SetPosition(Vector2{ WndAPI::kWidth_ / 2, 100.f });
    uiPtr_->GetUISpritePtr("playerUI_slash")->SetAlpha(1.f);
}

void PlayerUI::Update(void)
{
    // imgui
    //GUI::Begin("PlayerUI");
    //GUI::CheckBox("is_visible_", &is_visible_);
    //GUI::Text("rate:  %f", easeTimer_.GetTimeRate());
    //GUI::Text("frame: %f", easeTimer_.GetFrameCurrent());
    //GUI::End();

    const float snakeNum = (float)*rabbitCount_;
    const Vector2 point_snake = { 32.f * snakeNum, 0.f };
    uiPtr_->GetUISpritePtr("playerUI_figures")->SetCutStartPoint(point_snake);

    if (*coinCount_ < 10)
    {
        if (*coinCount_ < 5)
        {
            const float coinNum = (float)*coinCount_;
            const Vector2 point_coin = { 32.f * coinNum, 0.f };
            uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetCutStartPoint(point_coin);
        }
        else
        {
            const float coinNum = (float)(*coinCount_ - 5);
            const Vector2 point_coin = { 32.f * coinNum, 32.f };
            uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetCutStartPoint(point_coin);
        }
    }


    EaseUI();

    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Update("playerUI_starCount");
    uiPtr_->Update("playerUI_coinCount");
    uiPtr_->Update("playerUI_coinFigures");
    uiPtr_->Update("playerUI_starDustCount");
    uiPtr_->Update("playerUI_remain");
    uiPtr_->Update("playerUI_life");
    uiPtr_->Update("playerUI_figures");
    uiPtr_->Update("playerUI_figures2");
    uiPtr_->Update("playerUI_slash");

    // フラグ更新
    is_visiblePre_ = is_visible_;
}

void PlayerUI::Draw(void)
{
    // nullチェック
    if (!uiPtr_) { return; }

    uiPtr_->Draw("playerUI_starCount");
    uiPtr_->Draw("playerUI_coinCount");
    uiPtr_->Draw("playerUI_coinFigures");
    uiPtr_->Draw("playerUI_starDustCount");
    //uiPtr_->Draw("playerUI_remain");
    uiPtr_->Draw("playerUI_life");
    uiPtr_->Draw("playerUI_figures");
    uiPtr_->Draw("playerUI_figures2");
    uiPtr_->Draw("playerUI_slash");
}

void PlayerUI::EaseUI(void)
{
    if (IsTrigger() || IsRelease())
    {
        easeTimer_.Finish(true);
        easeTimer_.Start(0.6f);
        easeTimer_.SetAddSpeed(2.f);
    }

    if (is_visible_)
    {
        easeTimer_.Update();

        // 星の数の画像
        const Vector2& starCount_defPos = { kDefPos_left_, 40.f }; // 左 -> 右
        const float starCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), starCount_defPos.x, starCount_defPos.x + kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ starCount_posX,starCount_defPos.y });
        const float starCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetAlpha(starCount_alpha);

        // コインの数の画像
        const Vector2& coinCount_defPos = { kDefPos_right_, 580.f }; // 右 -> 左
        const float coinCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), coinCount_defPos.x, coinCount_defPos.x - kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ coinCount_posX,coinCount_defPos.y });
        const float coinCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAlpha(coinCount_alpha);

        // コインの数の数字
        const Vector2& coinFigures_defPos = { kDefPos_right_ + 100.f, 610.f }; // 右 -> 左
        const float coinFigures_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), coinFigures_defPos.x, coinFigures_defPos.x - kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetPosition(Vector2{ coinFigures_posX,coinFigures_defPos.y });
        const float coinFigures_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetAlpha(coinFigures_alpha);

        // 残機の数の画像
        const Vector2& remain_defPos = { kDefPos_left_ ,640.f }; // 左 -> 右
        const float remain_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), remain_defPos.x, remain_defPos.x + kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetPosition(Vector2{ remain_posX,remain_defPos.y });
        const float remain_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetAlpha(remain_alpha);

        // HPの画像
        const Vector2& life_defPos = { kDefPos_right_, 50.f }; // 右 -> 左
        const float life_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), life_defPos.x, life_defPos.x - kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ life_posX,life_defPos.y });
        const float life_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate());
        uiPtr_->GetUISpritePtr("playerUI_life")->SetAlpha(life_alpha);
    }
    else
    {
        easeTimer_.Update();

        // 星の数の画像
        const Vector2& starCount_defPos = { kDefPos_left_ + kMoveDist_, 40.f }; // 右 -> 左
        const float starCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), starCount_defPos.x, starCount_defPos.x - kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetPosition(Vector2{ starCount_posX,starCount_defPos.y });
        const float starCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_starCount")->SetAlpha(starCount_alpha);

        // コインの数の画像
        const Vector2& coinCount_defPos = { kDefPos_right_ - kMoveDist_, 580.f }; // 左 -> 右
        const float coinCount_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), coinCount_defPos.x, coinCount_defPos.x + kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetPosition(Vector2{ coinCount_posX,coinCount_defPos.y });
        const float coinCount_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_coinCount")->SetAlpha(coinCount_alpha);
        // コインの数の数字
        const Vector2& coinFigures_defPos = { kDefPos_right_ + 100.f, 610.f }; // 左 -> 右
        const float coinFigures_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), coinFigures_defPos.x, coinFigures_defPos.x + kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetPosition(Vector2{ coinFigures_posX,coinFigures_defPos.y });
        const float coinFigures_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_coinFigures")->SetAlpha(coinFigures_alpha);

        // 残機の数の画像
        const Vector2& remain_defPos = { kDefPos_left_ + kMoveDist_ ,640.f }; // 右 -> 左
        const float remain_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), remain_defPos.x, remain_defPos.x - kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetPosition(Vector2{ remain_posX,remain_defPos.y });
        const float remain_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_remain")->SetAlpha(remain_alpha);

        // HPの画像
        const Vector2& life_defPos = { kDefPos_right_ - kMoveDist_, 50.f }; // 左 -> 右
        const float life_posX = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), life_defPos.x, life_defPos.x + kMoveDist_);
        uiPtr_->GetUISpritePtr("playerUI_life")->SetPosition(Vector2{ life_posX,life_defPos.y });
        const float life_alpha = Math::Ease::EaseOutSine(easeTimer_.GetTimeRate(), 1.f, 0.f);
        uiPtr_->GetUISpritePtr("playerUI_life")->SetAlpha(life_alpha);
    }
}
