#pragma once
#include "IScene.h"
#include "FileSystem.h"
#include "Sprite.h"
#include "Object3D.h"

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
    std::unique_ptr<Object3D> obj3d_{ std::make_unique<Object3D>("Resources/cube/cube.obj") };
};

