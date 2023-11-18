#include "TitleScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "SimplifyImGui.h"
#include "UI.h"
#include "CameraManager.h"

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

    //
    dTimer_ease_logoUpScale_.Start(1.f);
    dTimer_ease_logoUpScale_.SetAddSpeed(2.f);
}

void TitleScene::Update(void)
{
    if (KEYS::IsTrigger(DIK_SPACE))
    {
        SceneManager::GetInstance()->RequestChangeScene(SceneName::DEMO);
    }

    UI::GetInstance()->Update("png_titleLogo-White");
    UI::GetInstance()->Update("png_titleLogo");
    UI::GetInstance()->Update("png_titleKana");

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

    GUI::Begin("Title_logo");
    GUI::Text("rate:  %f", rate);
    GUI::Text("scale: %f", scale);
    GUI::End();

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
}

void TitleScene::Draw2dBack(void)
{
}

void TitleScene::Finalize(void)
{
    CameraManager::GetInstance()->UnRegister(camera_title.get());
    UI::GetInstance()->UnRegister("png_titleLogo");
    UI::GetInstance()->UnRegister("png_titleKana");
}
