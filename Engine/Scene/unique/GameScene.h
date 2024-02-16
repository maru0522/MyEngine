#pragma once
#include "IScene.h"
#include <filesystem>
#include "Sprite.h"
#include "Object3D.h"
#include "Sound.h"
#include "LightManager.h"
#include "LevelData.h"
#include "Player.h"
#include "Planet.h"
#include "SphericalCamera.h"
#include "Rock.h"
#include "CollisionManager.h"
#include "IventArea_Tantative.h"
#include "StickyBall.h"
#include "Pipe.h"
#include "Coin.h"
#include "Event_StartTutorial.h"
#include "Event_EndTutorial.h"
#include "Event_TutorialPlanetHole.h"
#include "BackGroundTexture.h"
#include "BehindCamera.h"
#include "CoinList.h"
#include "Snake.h"
#include "ChickenEgg.h"
#include "GameManager.h"
#include "FigureUI.h"

class GameScene final :
    public IScene
{
private:
    // 定義
    using fsPath = std::experimental::filesystem::path;
    const float kCamDist_ = 80.f;
    const float kCamDiff_ = 10.f;

public:
    // 関数
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;

    void DrawBackGround(void);

    bool debugPlanetDraw_{ true }; // 星を描画する。
    bool debugSpherical_{ true }; // 球面座標系でカメラを管理する

    // 変数
    std::unique_ptr<LightManager> lightGroup_{ std::make_unique<LightManager>() };
    GameManager gameManager_;

    std::unique_ptr<Sprite> png_backGround_{ std::make_unique<Sprite>("Resources/BackGround.png") };
    bool isBG_;

    std::unique_ptr<Object3D> skydome_{ std::make_unique<Object3D>("Resources/model/skydome/skydome.obj") };

    std::unique_ptr<Rock> rock1_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock2_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock3_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock4_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock5_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };

    std::unique_ptr<Pipe> pipe1_{std::make_unique<Pipe>(CollisionManager::GetInstance()) };
    std::unique_ptr<Pipe> pipe2_{std::make_unique<Pipe>(CollisionManager::GetInstance()) };
};

