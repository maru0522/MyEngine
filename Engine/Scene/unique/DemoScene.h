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
#include "Rabbit.h"
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
#include "StoneBlock.h"

class DemoScene final :
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

    void DeployObj(LevelData* lvdPtr);
    void HotReload(LevelData* lvdPtr);

    void CameraSetUp(void);

    //bool debugCamFuncFollow_{ false }; // Follow
    //bool debugCamFollow_{ false }; // カメラとプレイヤーの距離が常に一定になるようにする
    //bool debugCamFollow2_{ }; // 軸をプレイヤーと同じにする。
    bool debugPlanetDraw_{ true }; // 星を描画する。
    bool debugSpherical_{ true }; // 球面座標系でカメラを管理する

    // 変数
    std::unique_ptr<LightManager> lightGroup_{ std::make_unique<LightManager>() };

    std::unique_ptr<Planet> planet_{ std::make_unique<Planet>() };
    std::unique_ptr<Player> player_{ std::make_unique<Player>(CameraManager::GetInstance(), CollisionManager::GetInstance(), lightGroup_.get(), planet_.get()) };
    std::unique_ptr<Rabbit> rabbit1_{ std::make_unique<Rabbit>(CollisionManager::GetInstance(), lightGroup_.get(), planet_.get()) };
    std::unique_ptr<Rabbit> rabbit2_{ std::make_unique<Rabbit>(CollisionManager::GetInstance(), lightGroup_.get(), planet_.get()) };
    std::unique_ptr<Rabbit> rabbit3_{ std::make_unique<Rabbit>(CollisionManager::GetInstance(), lightGroup_.get(), planet_.get()) };
    //std::unique_ptr<Object3D> testP_{ std::make_unique<Object3D>("Resources/model/lolP/lolP.obj") };
    std::unique_ptr<Sprite> png_backGround_{ std::make_unique<Sprite>("Resources/BackGround.png") };
    bool isBG_;

    std::unique_ptr<Object3D> skydome_{ std::make_unique<Object3D>("Resources/model/skydome/skydome.obj") };

    std::unique_ptr<ICamera> camera_debugPtr_{ std::make_unique<ICamera>() };
    std::unique_ptr<SphericalCamera> camera_colPtr_{ std::make_unique<SphericalCamera>("follow_player") };
    std::unique_ptr<BehindCamera> camera_behind_{ std::make_unique<BehindCamera>("follow_player") };
    std::unique_ptr<SphericalCamera> camera_4Hole_{ std::make_unique<SphericalCamera>("hole_planet") };
    //std::unique_ptr<Object3D> planet_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };

    //
    //std::unique_ptr<Object3D> hole_{ std::make_unique<Object3D>("Resources/model/cube/cube.obj") };
    //std::unique_ptr<IventArea_Tantative> hole1_{ std::make_unique<IventArea_Tantative>(CollisionManager::GetInstance(), "tunnel1") };
    //std::unique_ptr<IventArea_Tantative> hole2_{ std::make_unique<IventArea_Tantative>(CollisionManager::GetInstance(), "tunnel2") };
    //std::unique_ptr<Object3D> house_{ std::make_unique<Object3D>("Resources/model/cube/cube.obj") };
    std::unique_ptr<Rock> rock1_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock2_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock3_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock4_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    std::unique_ptr<Rock> rock5_{ std::make_unique<Rock>(CollisionManager::GetInstance()) };
    //std::unique_ptr<StickyBall> sticky1_{std::make_unique<StickyBall>(CollisionManager::GetInstance()) };

    std::unique_ptr<StoneBlock> stone_{std::make_unique<StoneBlock>(CollisionManager::GetInstance()) };

    std::unique_ptr<Pipe> pipe1_{std::make_unique<Pipe>(CollisionManager::GetInstance()) };
    std::unique_ptr<Pipe> pipe2_{std::make_unique<Pipe>(CollisionManager::GetInstance()) };

    //std::array<std::unique_ptr<Coin>, 19> coins_;
    std::unique_ptr<CoinList> coinList_;
    std::array<std::unique_ptr<Rabbit>, 3> rabbits_;

    // json読み込み&配置用
    std::map<std::string, std::unique_ptr<Object3D>> objects_;
    std::unique_ptr<LevelData> lvdPtr_;

    Event_StartTutorial a_{CameraManager::GetInstance(),player_.get() };
    Event_EndTutorial b_;
    Event_TutorialPlanetHole c_{ CollisionManager::GetInstance(),CameraManager::GetInstance(),player_.get()};

    //std::unique_ptr<Object3D> skyDivide_{ std::make_unique<Object3D>("Resources/model/plane/plane.obj") };
};

