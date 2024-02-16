#include "Input.h"
#include "DemoScene.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "Timer.h"
#include "SceneManager.h"
#include "UI.h"
#include "BackGroundTexture.h"
#include "Snake.h"

void GameScene::Initialize(void)
{
    const auto func_bgt = std::bind(&GameScene::DrawBackGround, this);
    BackGroundTexture::GetInstance()->Set(func_bgt);

    Object3D::SetLightGroup(lightGroup_.get());

    // 平行光源
    lightGroup_->SetLightActive(LightType::DIRECTIONAL, 0, true);
    lightGroup_->SetLightColor(LightType::DIRECTIONAL, 0, { 1,1,1 });
    lightGroup_->SetLightDir(LightType::DIRECTIONAL, 0, { 0,-1,0 });

    gameManager_.SetAllPtrs(CollisionManager::GetInstance(), CameraManager::GetInstance(), lightGroup_.get(),FigureUI::GetInstance(),UI::GetInstance());
    gameManager_.Initialize();

    rock1_->TransformPtr()->position = { 48,18,9 };
    rock2_->TransformPtr()->position = { 49,-23,6 };
    rock3_->TransformPtr()->position = { 48,-6,24 };
    rock4_->TransformPtr()->position = { 49,10,-20 };
    rock5_->TransformPtr()->position = { 48,-12,-15 };

    pipe1_->SetPosition({ 16,42.5f,20.f });
    pipe1_->SetRotation({ 0.497f,0.f,5.962f });
    pipe1_->SetPartnerPtr(pipe2_.get());
    pipe1_->GetColPushbackPtr()->SetID("pipe_enterInside1");
    pipe2_->SetPosition({ -29.855f,-38.f ,-11.024f });
    pipe2_->SetRotation({ 3.465f,0.f,0.621f });
    pipe2_->SetPartnerPtr(pipe1_.get());
    pipe2_->GetColPushbackPtr()->SetID("pipe_enterInside2");

    UI::GetInstance()->Register("circle_red", "Resources/circle_red.png");
    UI::GetInstance()->Register("circle_green", "Resources/circle_green.png");
}

void GameScene::Update(void)
{
    gameManager_.Update();

    const Vector3& dir = CameraManager::GetInstance()->GetCurrentCamera()->GetAxis3().forward;
    lightGroup_->SetLightDir(LightType::DIRECTIONAL, 0, dir);

    png_backGround_->Update();

    lightGroup_->Update();

    if (KEYS::IsTrigger(DIK_R)) { SceneManager::GetInstance()->RequestChangeScene(SceneName::TITLE); }
    //if (capturedRabbitCount == 3 && b_.GetIsExecite() == false) { b_.SetIsExecute(true); }

    rock1_->Update();
    rock2_->Update();
    rock3_->Update();
    rock4_->Update();
    rock5_->Update();

    //sticky1_->Update();
    pipe1_->Update();
    pipe2_->Update();

    skydome_->Update();

    UI::GetInstance()->Update("circle_red");
    UI::GetInstance()->Update("circle_green");
}

void GameScene::Draw3d(void)
{
    lightGroup_->Draw();
    gameManager_.Draw3d();

    rock1_->Draw();
    rock2_->Draw();
    rock3_->Draw();
    rock4_->Draw();
    rock5_->Draw();

    pipe1_->Draw();
    pipe2_->Draw();
    skydome_->Draw();
}


void GameScene::Draw2dFore(void)
{
    gameManager_.Draw2d();
}

void GameScene::Draw2dBack(void)
{
    if (isBG_) png_backGround_->Draw();
}

void GameScene::Finalize(void)
{
    gameManager_.Finalize();
}

void GameScene::DrawBackGround(void)
{
    skydome_->Draw();
}