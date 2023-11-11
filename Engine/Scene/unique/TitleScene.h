#pragma once
#include "IScene.h"
#include "Sprite.h"
#include "ICamera.h"
#include "DebugCamera.h"

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

    std::unique_ptr<ICamera> camera_title{ std::make_unique<ICamera>("titleScene") };
    std::unique_ptr<DebugCamera> camera_ttttt{ std::make_unique<DebugCamera>("tttttt") };
    //std::unique_ptr<Sprite> png_logo_ = std::make_unique<Sprite>("Resources/SUPER_MARUYAMA_GALAXY.png");
};

