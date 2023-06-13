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
    //obj3d_->Update();
    obj3d2_->Update();
    Vector3 rot = obj3d2_->coordinate_.GetRotation();
    rot.y += 0.05f;
    obj3d2_->coordinate_.SetRotation(rot);
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
