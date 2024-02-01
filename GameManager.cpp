#include "GameManager.h"

void GameManager::SetAllPtrs(CollisionManager* arg_colMPtr, CameraManager* arg_camMPtr, LightManager* arg_lightMPtr)
{
    colMPtr_ = arg_colMPtr;
    camMPtr_ = arg_camMPtr;
    lightMPtr_ = arg_lightMPtr;
}

void GameManager::Initialize()
{
#pragma region オブジェクト
    // 惑星の初期化
    tutorialPlanet_.Initialize(colMPtr_);
    // プレイヤーの初期化
    player_.Initialize(camMPtr_, colMPtr_, lightMPtr_, &tutorialPlanet_);
    player_.SetupLightCircleShadows();
#pragma endregion

#pragma region イベント
    event_startTutorial_.Initialize(camMPtr_, &player_);
    event_endTutorial_.Initialize(camMPtr_);
    event_tutorialPlanetHole_.SetIsExecute(true);

    // startTutorialから起動する。 endTutorialは終了時,tutorialPlanetHoleは、実行途中で起動する
    event_startTutorial_.SetIsExecute(true);
#pragma endregion
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
    event_tutorialPlanetHole_.SetIsExecute(true);

    // startTutorialから起動する。 endTutorialは終了時,tutorialPlanetHoleは、実行途中で起動する
    event_startTutorial_.SetIsExecute(true);
#pragma endregion
}
