#pragma once
#include "IScene.h"
#include "FileSystem.h"
#include "Sprite.h"
#include "Object3D.h"
#include "Sound.h"
#include "LightGroup.h"
#include "LevelData.h"
#include "Player.h"
#include "Planet.h"
#include "Collision.h"

class DemoScene :
    public IScene
{
private:
    // 定義
    using fsPath = std::experimental::filesystem::path;

public:
    // 関数
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;

    void DeployObj(LevelData* lvdPtr);
    void HotReload(LevelData* lvdPtr);

    void DemoCollision(Player* player, Planet* planet);

    void DebudGui(void);
    bool debugCamFollow_;
    bool debugPlanetDraw_{ true };

    // 変数
    std::unique_ptr<Player> player_{ std::make_unique<Player>() };
    std::unique_ptr<Planet> planet_{ std::make_unique<Planet>() };
    //std::unique_ptr<Object3D> planet_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };

    // json読み込み&配置用
    std::map<std::string, std::unique_ptr<Object3D>> objects_;
    std::unique_ptr<LevelData> lvdPtr_;

    std::unique_ptr<Camera> cameraPtr{ std::make_unique<Camera>() };
    std::unique_ptr<LightGroup> lightGroup_{ std::make_unique<LightGroup>() };
};

