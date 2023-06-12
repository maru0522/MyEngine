#include "DemoScene.h"
#include "Input.h"

void DemoScene::Initialize(void)
{
    // カメラの座標設定
    cameraPtr->SetPosition({ 0,0,-20 });
    // カメラをマネージャーにセット
    CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());
}

void DemoScene::Update(void)
{
    sprite_->Update();
    obj3d_->Update();
    obj3d2_->Update();

    if (KEYS::IsTrigger(DIK_P)) sound_->Play(1);
    if (KEYS::IsTrigger(DIK_O)) music_->Play();
    if (KEYS::IsTrigger(DIK_I)) music_->Stop();
    if (KEYS::IsTrigger(DIK_U)) music_->SetVolume(0.1f);
    if (KEYS::IsTrigger(DIK_Y)) music3_->Play();
}

void DemoScene::Draw3d(void)
{
    //obj3d_->Draw();
    obj3d2_->Draw();
    // cubeの画像にスプライトで表示しているものを起用
    //obj3d_->Draw("Resources/namida.png");
}

void DemoScene::Draw2d(void)
{
    sprite_->Draw();
}

void DemoScene::Finalize(void)
{
}
