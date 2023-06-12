#pragma once
#include "IScene.h"
#include "FileSystem.h"
#include "Sprite.h"
#include "Object3D.h"
#include "Sound.h"

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
    void Draw2d(void) override;
    void Finalize(void) override;

    // 変数
    std::unique_ptr<Sprite> sprite_{ std::make_unique<Sprite>("Resources/namida.png") };
    std::unique_ptr<Object3D> obj3d_{ std::make_unique<Object3D>("Resources/model/cube/cube.obj") };
    std::unique_ptr<Object3D> obj3d2_{ std::make_unique<Object3D>("Resources/model/sphere/sphere.obj") };
    std::unique_ptr<Sound> sound_{ std::make_unique<Sound>("Resources/sound/Windows User Account Control.wav") };
    std::unique_ptr<Sound> music_{ std::make_unique<Sound>("Resources/sound/champion.wav") };
    std::unique_ptr<Sound> music3_{ std::make_unique<Sound>("Resources/sound/championM.mp3") };
    //std::unique_ptr<Sound> music2_{ std::make_unique<Sound>("Resources/sound/edm.wav") };
    std::unique_ptr<Camera> cameraPtr{ std::make_unique<Camera>() };
};

