#include "TitleScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "SimplifyImGui.h"
#include "UI.h"

void TitleScene::Initialize(void)
{
    //>> カメラのデバッグカメラモードをON
    camera_debugPtr_->SetIsDebugMode(true); // デバッグカメラ

    CameraManager::GetInstance()->SetCurrentCamera(camera_debugPtr_.get());

    UI::GetInstance()->Register("png_titleLogo", "Resources/SUPER_MARUYAMA_GALAXY.png");
}

void TitleScene::Update(void)
{
    if (KEYS::IsTrigger(DIK_SPACE))
    {
        SceneManager::GetInstance()->RequestChangeScene(SceneName::DEMO);
    }

    GUI::Begin("Text");
    GUI::Text("key-space : go next");
    GUI::End();

    UI::GetInstance()->Update("png_titleLogo");
}

void TitleScene::Draw3d(void)
{
}

void TitleScene::Draw2dFore(void)
{
    UI::GetInstance()->Draw("png_titleLogo");
}

void TitleScene::Draw2dBack(void)
{
}

void TitleScene::Finalize(void)
{
}
