#pragma once
#include "IScene.h"
#include "Sprite.h"
#include "CameraManager.h"

class TitleScene final :
    public IScene
{
public:
    // 関数
    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;

    std::unique_ptr<Camera> camera_debugPtr_{ std::make_unique<Camera>() };
    std::unique_ptr<Sprite> png_logo_ = std::make_unique<Sprite>("Resources/SUPER_MARUYAMA_GALAXY.png");
};

