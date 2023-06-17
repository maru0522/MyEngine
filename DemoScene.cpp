#include "DemoScene.h"
#include "Input.h"

void DemoScene::Initialize(void)
{
    // カメラの座標設定
    cameraPtr->eye_.z = -20;
    // カメラのデバッグカメラモードをON
    cameraPtr->SetIsDebugMode(true);
    // カメラをマネージャーにセット
    CameraManager::GetInstance()->SetCurrentCamera(cameraPtr.get());

    //lightGroup_->SetDirLightColor({ 1,1,1 });
    //lightGroup_->SetLightDir({ 0,1,5 });
    Object3D::SetLightGroup(lightGroup_.get());
}

void DemoScene::Update(void)
{
    lightGroup_->Update();

    sprite_->Update();
    //obj3d_->Update();
    obj3d2_->Update();
    Vector3 rot = obj3d2_->coordinate_.GetRotation();
    rot.y += 0.05f;
    obj3d2_->coordinate_.SetRotation(rot);
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();
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
