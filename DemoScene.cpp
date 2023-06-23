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

    //sprite_->SetSize({500,500});
    Object3D::SetLightGroup(lightGroup_.get());

    obj3d_->coordinate_.SetScale({ 20.f,0.5f,20.f });
    obj3d_->coordinate_.SetPosition({ 0.f,-5.f,0.f });
}

void DemoScene::Update(void)
{
    lightGroup_->Update();

    sprite_->Update();
    obj3d_->Update();
    obj3d2_->Update();
    Vector3 rot = obj3d2_->coordinate_.GetRotation();
    rot.y += 0.05f;
    obj3d2_->coordinate_.SetRotation(rot);
}

void DemoScene::Draw3d(void)
{
    lightGroup_->Draw();
    obj3d_->Draw();
    obj3d2_->Draw();
    // cubeの画像にスプライトで表示しているものを起用
    //obj3d_->Draw("Resources/namida.png");
}


void DemoScene::Draw2dFore(void)
{
}

void DemoScene::Draw2dBack(void)
{
    //sprite_->Draw();
}

void DemoScene::Finalize(void)
{
}
