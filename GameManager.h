#pragma once
#include "CollisionManager.h"
#include "CameraManager.h"
#include "LightManager.h"

#include "Planet.h"
#include "Player.h"
#include "ChickenEgg.h"

#include "CoinList.h"

#include "BehindCamera.h"
#include "SphericalCamera.h"

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
    //
    void CamerasSetting(void);

    //>> 変数
    CollisionManager* colMPtr_;
    CameraManager* camMPtr_;
    LightManager* lightMPtr_;

    // カメラ
    std::unique_ptr<ICamera> camera_debugPtr_{ std::make_unique<ICamera>() };
    std::unique_ptr<SphericalCamera> camera_colPtr_{ std::make_unique<SphericalCamera>("follow_player") };
    std::unique_ptr<BehindCamera> camera_behind_{ std::make_unique<BehindCamera>("follow_player") };
    std::unique_ptr<SphericalCamera> camera_4Hole_{ std::make_unique<SphericalCamera>("hole_planet") };

    // オブジェクト
    Planet tutorialPlanet_;
    Player player_;
    ChickenEgg chickenEgg_;
    std::array<Snake, 3> snakes_;

    CoinList coinList_;

    // イベント
    Event_StartTutorial event_startTutorial_;
    Event_EndTutorial event_endTutorial_;
    Event_TutorialPlanetHole event_tutorialPlanetHole_;
};