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
#include "ColliderCamera.h"
#include "Rabbit.h"

class DemoScene final :
    public IScene
{
private:
    // ��`
    using fsPath = std::experimental::filesystem::path;
    const float kCamDist_ = 80.f;
    const float kCamDiff_ = 10.f;

public:
    // �֐�
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;

    void DeployObj(LevelData* lvdPtr);
    void HotReload(LevelData* lvdPtr);

    //bool debugCamFuncFollow_{ false }; // Follow
    //bool debugCamFollow_{ false }; // �J�����ƃv���C���[�̋�������Ɉ��ɂȂ�悤�ɂ���
    //bool debugCamFollow2_{ }; // �����v���C���[�Ɠ����ɂ���B
    bool debugPlanetDraw_{ true }; // ����`�悷��B
    bool debugSpherical_{ true }; // ���ʍ��W�n�ŃJ�������Ǘ�����

    // �ϐ�
    std::unique_ptr<Player> player_{ std::make_unique<Player>(CameraManager::GetInstance()) };
    std::unique_ptr<Rabbit> rabbit1_{ std::make_unique<Rabbit>() };
    std::unique_ptr<Planet> planet_{ std::make_unique<Planet>() };
    std::unique_ptr<Object3D> testP_{ std::make_unique<Object3D>("Resources/model/lolP/lolP.obj") };
    std::unique_ptr<Sprite> testSprite_{ std::make_unique<Sprite>("Resources/reimu.png") };
    //std::unique_ptr<Object3D> planet_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };

    //
    std::unique_ptr<Object3D> hole_{ std::make_unique<Object3D>("Resources/model/cube/cube.obj") };
    std::unique_ptr<Object3D> house_{ std::make_unique<Object3D>("Resources/model/cube/cube.obj") };

    // json�ǂݍ���&�z�u�p
    std::map<std::string, std::unique_ptr<Object3D>> objects_;
    std::unique_ptr<LevelData> lvdPtr_;

    std::unique_ptr<Camera> cameraPtr{ std::make_unique<Camera>() };
    std::unique_ptr<ColliderCamera> colCameraPtr{ std::make_unique<ColliderCamera>(CollisionManager::GetInstance()) };
    std::unique_ptr<LightGroup> lightGroup_{ std::make_unique<LightGroup>() };
};

