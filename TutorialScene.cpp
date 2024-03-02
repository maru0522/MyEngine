#include "TutorialScene.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "Object3D.h"

void TutorialScene::Initialize(void)
{
    // ptr
    auto camMPtr = CameraManager::GetInstance();
    auto colMPtr = CollisionManager::GetInstance();

    // 平行光源
    Object3D::SetLightGroup(lightGroup_.get());
    lightGroup_->SetLightActive(LightType::DIRECTIONAL, 0, true);
    lightGroup_->SetLightColor(LightType::DIRECTIONAL, 0, { 1,1,1 });
    lightGroup_->SetLightDir(LightType::DIRECTIONAL, 0, { 0,-1,0 });

    // planet
    tutorialPlanet_.Initialize(colMPtr);
    // プレイヤー
    player_.Initialize(camMPtr, colMPtr, lightGroup_.get(), &tutorialPlanet_);

    // カメラ
    camMPtr->Register(camera_tutorial_.get());
    camera_tutorial_->SetTransform(Transform{ { 0,0,0 }, camera_tutorial_->GetTransform().rotation, camera_tutorial_->GetTransform().scale });        // プレイヤー用カメラ2の座標
    camMPtr->SetCurrentCamera(camera_tutorial_.get());

    // 鶏卵の初期化
    chickenEgg_.Initialize(colMPtr, lightGroup_.get(), &tutorialPlanet_);
    // 蛇
    snake_.Initialize(colMPtr, lightGroup_.get(), &tutorialPlanet_, &chickenEgg_);
    // 蛇用ケージ
    snakeCage_.Initialize(colMPtr, lightGroup_.get(), &tutorialPlanet_);
}

void TutorialScene::Update(void)
{
    // ライト
    // 平行光源の向きをカメラの正面ベクトル方向に。
    const Vector3& dir = CameraManager::GetInstance()->GetCurrentCamera()->GetAxis3().forward;
    lightGroup_->SetLightDir(LightType::DIRECTIONAL, 0, dir);
    lightGroup_->Update();

    // オブジェクト
    player_.Update();
    tutorialPlanet_.Update();
    chickenEgg_.Update();
    snake_.Update();
    snakeCage_.Update();
}

void TutorialScene::Draw3d(void)
{
    lightGroup_->Draw();

    player_.Draw3d();
    tutorialPlanet_.Draw();
    chickenEgg_.Draw();
    snake_.Draw();
    snakeCage_.Draw();
}

void TutorialScene::Draw2dFore(void)
{
}

void TutorialScene::Draw2dBack(void)
{
}

void TutorialScene::Finalize(void)
{
    // カメラ
    auto camMPtr = CameraManager::GetInstance();
    camMPtr->UnRegister(camera_tutorial_.get());

    // オブジェクト
    player_.Finalize();
    tutorialPlanet_.Finalize();
    snake_.Finalize();
    snakeCage_.Finalize();
}
