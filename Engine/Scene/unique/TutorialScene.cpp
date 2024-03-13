#include "TutorialScene.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "Object3D.h"
#include "Input.h"
#include "SceneManager.h"

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
    chickenEgg_.SetPosition(Vector3{ 0,25,45 });
    chickenEgg_.SetEggNum(4);

    // 蛇
    snake_.Initialize(colMPtr, lightGroup_.get(), &tutorialPlanet_, &chickenEgg_);
    snake_.GetTransformPtr()->position = Vector3{ 0,50,15 };
    // 蛇用ケージ
    snakeCage_.Initialize(colMPtr, lightGroup_.get(), &tutorialPlanet_);
    snakeCage_.SetPosition(Vector3{ 50,8,10 });

    event_tutorial_startCamera_.Initialize(camMPtr, &player_, &snake_.GetTransformPtr()->position, snake_.GetPosturePtr(), &isSnakeUpdate_);
    event_tutorial_startCamera_.SetIsExecute(true);
}

void TutorialScene::Update(void)
{
    if (KEYS::IsTrigger(DIK_R)) { SceneManager::GetInstance()->RequestChangeScene(SceneName::TITLE); }
    if (KEYS::IsTrigger(DIK_P)) { SceneManager::GetInstance()->RequestChangeScene(SceneName::GAME); }

    // ライト
    // 平行光源の向きをカメラの正面ベクトル方向に。
    const Vector3& dir = CameraManager::GetInstance()->GetCurrentCamera()->GetAxis3().forward;
    lightGroup_->SetLightDir(LightType::DIRECTIONAL, 0, dir);
    lightGroup_->Update();

    // オブジェクト
    player_.Update();
    tutorialPlanet_.Update();
    chickenEgg_.Update();
    if (isSnakeUpdate_) { snake_.Update(); }
    snakeCage_.Update();

    SnakeIntoCustody();

    event_tutorial_startCamera_.Execute();
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
    event_tutorial_startCamera_.Draw();
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

    event_tutorial_startCamera_.Finalize();
}

void TutorialScene::SnakeIntoCustody(void)
{
    // 蛇が捕獲可能な状況にあるケージがあるか
    bool is_capturableCage{};
    // ケージの座標ptr（蛇が捕まっている時用）
    Vector3* cagePosPtr{};
    // getter呼び出し
    is_capturableCage = snakeCage_.GetIsCapture();
    // falseの場合スキップ
    if (is_capturableCage == false) { return; }

    // trueの場合↓
    // この檻は捕獲したため、以降使用不可
    snakeCage_.SetIsLock(true);
    // ケージの座標ptrを受け取る。
    cagePosPtr = snakeCage_.GetPosPtr();

    // 蛇が捕獲されたかを確認
        // getter呼び出して確認
    bool is_touchCaged = snake_.GetIsTouchCage();
    // falseの場合、スキップ
    if (is_touchCaged == false) { return; }

    // trueの場合↓
    // 蛇にケージの座標ptrを渡す。
    snake_.Caged(cagePosPtr);
    SceneManager::GetInstance()->RequestChangeScene(SceneName::GAME);
}
