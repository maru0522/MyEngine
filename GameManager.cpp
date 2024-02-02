#include "GameManager.h"

void GameManager::SetAllPtrs(CollisionManager* arg_colMPtr, CameraManager* arg_camMPtr, LightManager* arg_lightMPtr)
{
    colMPtr_ = arg_colMPtr;
    camMPtr_ = arg_camMPtr;
    lightMPtr_ = arg_lightMPtr;
}

void GameManager::Initialize()
{
    //>> オブジェクト
    // カメラの設定
    CamerasSetting();
    // 惑星の初期化
    tutorialPlanet_.Initialize(colMPtr_);
    // プレイヤーの初期化
    player_.Initialize(camMPtr_, colMPtr_, lightMPtr_, &tutorialPlanet_);
    player_.SetupLightCircleShadows();
    // 鶏卵の初期化
    chickenEgg_.Initialize(colMPtr_, lightMPtr_, &tutorialPlanet_);
    // 蛇
    for (auto& snake : snakes_)
    {
        snake.Initialize(colMPtr_, lightMPtr_, &tutorialPlanet_, &chickenEgg_);
        snake.SetupLightCircleShadows();
    }
    snakes_[1].GetTransformPtr()->position = { 10,60, 20 };
    snakes_[2].GetTransformPtr()->position = { -10,60, 20 };

    // コインリストの初期化とコインの配置
    coinList_.Initialize(colMPtr_);
    coinList_.DeployCoin("Resources/externalText/coinsLayout.txt", &tutorialPlanet_, lightMPtr_);

    //>> イベント
    event_startTutorial_.Initialize(camMPtr_, &player_);
    event_endTutorial_.Initialize(camMPtr_);
    event_tutorialPlanetHole_.Initialize(colMPtr_, camMPtr_, &player_);

    // startTutorialから起動する。 endTutorialは終了時,tutorialPlanetHoleは、更新処理内で起動する
    event_startTutorial_.SetIsExecute(true);
}

void GameManager::Update(void)
{
}

void GameManager::Finalize(void)
{
    // 惑星の終了処理
    tutorialPlanet_.Finalize();
    // プレイヤーの終了処理
    player_.Finalize();

#pragma region イベント
    event_startTutorial_.Finalize();
    event_endTutorial_.Finalize();
    event_tutorialPlanetHole_.Finalize();

    // startTutorialから起動する。 endTutorialは終了時,tutorialPlanetHoleは、実行途中で起動する
    event_startTutorial_.SetIsExecute(true);
#pragma endregion
}

void GameManager::CamerasSetting(void)
{
    //>> カメラの座標設定
    camMPtr_->Register(camera_debugPtr_.get());
    camera_debugPtr_->SetTransform(Transform{ { 0,0,-70 }, camera_debugPtr_->GetTransform().rotation, camera_debugPtr_->GetTransform().scale });   // デバッグカメラの座標
    camMPtr_->Register(camera_colPtr_.get());
    camera_colPtr_->SetTransform(Transform{ { 3,172,-3 }, camera_colPtr_->GetTransform().rotation, camera_colPtr_->GetTransform().scale });        // プレイヤー用カメラの座標
    camMPtr_->Register(camera_behind_.get());
    camera_colPtr_->SetTransform(Transform{ { 3,172,-3 }, camera_colPtr_->GetTransform().rotation, camera_colPtr_->GetTransform().scale });        // プレイヤー用カメラ2の座標
    camMPtr_->Register(camera_4Hole_.get());
    camera_4Hole_->SetTransform(Transform{ { 0, 190, 0 }, { 1.5725f,-1.2175f,0} , camera_4Hole_->GetTransform().scale });                          // 穴に落ちたとき用カメラの座標と回転
}
