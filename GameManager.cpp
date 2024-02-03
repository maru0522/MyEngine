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
    //>> オブジェクト
    // 鶏卵付近の蛇数をプレイヤーへ渡す
    HandoverSnakeCount();
    // 鶏卵更新
    chickenEgg_.Update();

    // プレイヤー更新
    player_.Update();
    // 惑星の更新
    tutorialPlanet_.Update();
    // 蛇の更新
    for (auto& snake : snakes_) { snake.Update(); }
    // コインリストの更新
    coinList_.Update();

    //>> イベント
    event_startTutorial_.Execute();
    event_endTutorial_.Execute();
    event_tutorialPlanetHole_.Execute();
}

void GameManager::Draw3d(void)
{
    //>> オブジェクト
    // 鶏卵描画
    chickenEgg_.Draw();
    // プレイヤー描画
    player_.Draw3d();
    // 惑星の描画
    tutorialPlanet_.Draw();
    // 蛇の描画
    for (auto& snake : snakes_) { snake.Draw(); }
    // コインリストの描画
    coinList_.Draw();

    event_tutorialPlanetHole_.Draw();
}

void GameManager::Draw2d(void)
{
    player_.Draw2dFore();

    event_startTutorial_.Draw();
    event_endTutorial_.Draw();
}

void GameManager::Finalize(void)
{
    //>> オブジェクト
    // 惑星の終了処理
    tutorialPlanet_.Finalize();
    // プレイヤーの終了処理
    player_.Finalize();

    // カメラ
    camMPtr_->UnRegister(camera_debugPtr_.get());
    camMPtr_->UnRegister(camera_colPtr_.get());
    camMPtr_->UnRegister(camera_behind_.get());
    camMPtr_->UnRegister(camera_4Hole_.get());

    //>> イベント
    event_startTutorial_.Finalize();
    event_endTutorial_.Finalize();
    event_tutorialPlanetHole_.Finalize();

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

void GameManager::HandoverSnakeCount(void)
{
    // 蛇の数を取得
    int32_t snakeCount = chickenEgg_.GetApproachingEggSnakes();
    // プレイヤーへ設定
    player_.SetApproachingEggSnakes(snakeCount);
}
