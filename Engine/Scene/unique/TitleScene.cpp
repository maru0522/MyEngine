#include "TitleScene.h"
#include "Input.h"
#include "SceneManager.h"
#include "SimplifyImGui.h"

void TitleScene::Initialize(void)
{
    //>> カメラのデバッグカメラモードをON
    camera_debugPtr_->SetIsDebugMode(true); // デバッグカメラ

    CameraManager::GetInstance()->SetCurrentCamera(camera_debugPtr_.get());
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

    png_logo_->Update();
}

void TitleScene::Draw3d(void)
{
}

void TitleScene::Draw2dFore(void)
{
    png_logo_->Draw();
}

void TitleScene::Draw2dBack(void)
{
    png_logo_->Draw();
}

void TitleScene::Finalize(void)
{
}
