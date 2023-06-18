#pragma once
#include "IScene.h"
#include "FileSystem.h"
#include "Sprite.h"
#include "Object3D.h"
#include "Sound.h"
#include "LightGroup.h"

class DemoScene :
    public IScene
{
private:
    // íËã`
    using fsPath = std::experimental::filesystem::path;

public:
    // ä÷êî
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2d(void) override;
    void Finalize(void) override;

    // ïœêî
    std::unique_ptr<Sprite> sprite_{ std::make_unique<Sprite>("Resources/namida.png") };
    std::unique_ptr<Object3D> obj3d_{ std::make_unique<Object3D>("Resources/model/cube/cube.obj") };
    std::unique_ptr<Object3D> obj3d2_{ std::make_unique<Object3D>("Resources/model/ICOSphere/ICOSphere.obj") };
    //std::unique_ptr<Sound> music2_{ std::make_unique<Sound>("Resources/sound/edm.wav") };

    std::unique_ptr<Camera> cameraPtr{ std::make_unique<Camera>() };
    std::unique_ptr<LightGroup> lightGroup_{ std::make_unique<LightGroup>() };
};

