#include "TitleScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "SimplifyImGui.h"
#include "UI.h"
#include "CameraManager.h"
#include "FigureUI.h"

void TitleScene::Initialize(void)
{
    //CameraManager::GetInstance()->SetCurrentCamera(camera_debugPtr_.get());
    CameraManager::GetInstance()->Register(camera_title.get());
    CameraManager::GetInstance()->SetCurrentCamera(camera_title.get());

    UI::GetInstance()->Register("png_titleLogo-White", "Resources/title_logo_white.png");
    UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetPosition(Vector2{ 640.f,360.f });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetColor(Vector4{ 1.f,1.f,1.f,1.f });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetScale(Vector2{ 0.f,0.f });


    UI::GetInstance()->Register("png_titleLogo", "Resources/title_logo.png");
    UI::GetInstance()->GetUISpritePtr("png_titleLogo")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo")->SetPosition(Vector2{ 640.f,360.f });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo")->SetColor(Vector4{ 1.f,1.f,1.f,1.f });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo")->SetScale(Vector2{ 0.f,0.f });

    UI::GetInstance()->Register("png_titleKana", "Resources/title_kana.png");
    UI::GetInstance()->GetUISpritePtr("png_titleKana")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    UI::GetInstance()->GetUISpritePtr("png_titleKana")->SetPosition(Vector2{ 640.f,360.f });

    UI::GetInstance()->Register("png_titlePressA", "Resources/title_press_A.png");
    UI::GetInstance()->GetUISpritePtr("png_titlePressA")->SetAnchorPoint(Vector2{ 0.5f,0.5f });
    UI::GetInstance()->GetUISpritePtr("png_titlePressA")->SetPosition(Vector2{ 640.f,660.f });
    UI::GetInstance()->GetUISpritePtr("png_titlePressA")->SetAlpha(0.f);

    //
    dTimer_ease_logoUpScale_.Start(1.f);
    dTimer_ease_logoUpScale_.SetAddSpeed(2.f);
    dTimer_ease_pressAStrAlpha_.Start(4.f);
    dTimer_ease_pressAStrAlpha_.SetAddSpeed(2.f);

    FigureUI::GetInstance()->Register("figureUI_test");
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test")->pos = { 200,200 };
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test")->num = 1.5;
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test")->scope = FigureUI::Scope::MAX;


    FigureUI::GetInstance()->Register("figureUI_test_second");
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_second")->pos = { 200,300 };
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_second")->num = 1.5;

    FigureUI::GetInstance()->Register("figureUI_test_timer");
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->pos = { 200,400 };
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->num = 1.5;
    FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->format = FigureUI::Format::SCORE;
}

void TitleScene::Update(void)
{
    if (KEYS::IsTrigger(DIK_SPACE) || XPAD::IsTrigger(XPAD::Button::A))
    {
        SceneManager::GetInstance()->RequestChangeScene(SceneName::GAME);
        return;
    }

    UI::GetInstance()->Update("png_titleLogo-White");
    UI::GetInstance()->Update("png_titleLogo");
    UI::GetInstance()->Update("png_titleKana");
    UI::GetInstance()->Update("png_titlePressA");

    dTimer_ease_logoUpScale_.Update();
    const float rate = dTimer_ease_logoUpScale_.GetTimeRate();
    //const float scale = rate * rate;
    const float scale = Math::Ease::EaseInCubic(rate);
    const float scale_white = Math::Ease::EaseInCubic(rate);
    UI::GetInstance()->GetUISpritePtr("png_titleLogo")->SetScale(Vector2{ scale,scale });
    UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetScale(Vector2{ scale_white,scale_white });

    static float pre_rate;
    if (pre_rate < 1.f && rate > 1.f)
    {
        dTimer_ease_WhitelogoDownScale_.Start(0.5f);
    }
    dTimer_ease_WhitelogoDownScale_.Update();

    if (rate > 0.8f)
    {
        const float newScale = 1 - rate * rate;
        UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetScale(Vector2{ newScale,newScale });
        UI::GetInstance()->GetUISpritePtr("png_titleLogo-White")->SetAlpha(newScale);
    }

    dTimer_ease_pressAStrAlpha_.Update();
    const float alpha_rate = dTimer_ease_pressAStrAlpha_.GetTimeRate();
    const float alpha_pressA = Math::Ease::EaseInSine(alpha_rate);
    UI::GetInstance()->GetUISpritePtr("png_titlePressA")->SetAlpha(alpha_pressA);

#ifdef _DEBUG
    GUI::Begin("Title_logo");
    GUI::Text("rate:  %f", rate);
    GUI::Text("scale: %f", scale);

    GUI::SliderFloat("num", &FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test")->num, 0, 3000);
    GUI::SliderFloat("num2", &FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_second")->num, 0, 3000);
    GUI::SliderFloat("num3", &FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->num, 0, 3000);
    ImGui::SameLine();
    if (GUI::ButtonTrg("SwitchFormat"))
    {
        FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->format == FigureUI::Format::TIMER ?
            FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->format = FigureUI::Format::SCORE :
            FigureUI::GetInstance()->GetFigureUISettingsPtr("figureUI_test_timer")->format = FigureUI::Format::TIMER;
    }
    GUI::End();
#endif

    // 1frame前の値として記録
    pre_rate = rate;
}

void TitleScene::Draw3d(void)
{
}

void TitleScene::Draw2dFore(void)
{
    UI::GetInstance()->Draw("png_titleKana");
    UI::GetInstance()->Draw("png_titleLogo");
    UI::GetInstance()->Draw("png_titleLogo-White");
    UI::GetInstance()->Draw("png_titlePressA");
}

void TitleScene::Draw2dBack(void)
{
}

void TitleScene::Finalize(void)
{
    CameraManager::GetInstance()->UnRegister(camera_title.get());
    UI::GetInstance()->UnRegister("png_titleLogo");
    UI::GetInstance()->UnRegister("png_titleKana");
    UI::GetInstance()->UnRegister("png_titleKana");
    UI::GetInstance()->UnRegister("png_titleKana");

    // タイマーの停止
    dTimer_ease_logoUpScale_.Finish();
    dTimer_ease_WhitelogoDownScale_.Finish();
    dTimer_ease_pressAStrAlpha_.Finish();
}
