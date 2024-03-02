#pragma once
#include "IScene.h"
#include "ICamera.h"
#include "BehindCamera.h"
#include <memory>
#include "Player.h"
#include "LightManager.h"
#include "GamePlanet.h"
#include "ChickenEgg.h"
#include "Snake.h"
#include "SnakeCage.h"
#include "Event_Tutorial_StartCamera.h"

class TutorialScene final : public IScene
{
public:
    //>> 関数
    TutorialScene(void) = default;
    ~TutorialScene(void) override = default;

    void Initialize(void) override;
    void Update(void) override;
    void Draw3d(void) override;
    void Draw2dFore(void) override;
    void Draw2dBack(void) override;
    void Finalize(void) override;

private:
    void SnakeIntoCustody(void);

    //>> 変数
    std::unique_ptr<BehindCamera> camera_tutorial_{ std::make_unique<BehindCamera>("for_tutorial") };

    std::unique_ptr<LightManager> lightGroup_{ std::make_unique<LightManager>() };
    IPlanet tutorialPlanet_;

    Player player_;
    ChickenEgg chickenEgg_;
    Snake snake_;
    bool isSnakeUpdate_;
    SnakeCage snakeCage_;

    Event_Tutorial_StartCamera event_tutorial_startCamera_;
};

