#pragma once
#include "CollisionManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "Planet.h"
#include "Player.h"

#include "Event_StartTutorial.h"
#include "Event_EndTutorial.h"
#include "Event_TutorialPlanetHole.h"

class GameManager
{
public:
    //>> 関数
    GameManager(void) = default;
    ~GameManager(void) = default;

    // 必要なptrを設定
    void SetAllPtrs(CollisionManager* arg_colMPtr, CameraManager* arg_camMPtr, LightManager* arg_lightMPtr);
    // 初期化関数
    void Initialize(void);
    void Update(void);
    void Finalize(void);

private:
    //>> 変数
    CollisionManager* colMPtr_;
    CameraManager* camMPtr_;
    LightManager* lightMPtr_;

    // オブジェクト
    Planet tutorialPlanet_;
    Player player_;

    // イベント
    Event_StartTutorial event_startTutorial_;
    Event_EndTutorial event_endTutorial_;
    Event_TutorialPlanetHole event_tutorialPlanetHole_;
};
