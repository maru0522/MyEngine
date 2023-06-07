#pragma once
#include <memory>
#include "IScene.h"
#include "SceneFactory.h"

class SceneManager
{
public:
    // ä÷êî
    SceneManager(void);
    ~SceneManager(void);

    void RequestChangeScene(std::unique_ptr<IScene>& nextScene, int32_t waitFrame = 0);

    void Initialize(SceneFactory::Usage firstScene = SceneFactory::Usage::DEMO);
    void Update(void);
    void Draw3d(void);
    void Draw2d(void);

private:
    // ïœêî
    int32_t waitFrame_{};
    std::unique_ptr<IScene> currentScene_{ nullptr };
    std::unique_ptr<IScene> nextScene_{ nullptr };

    std::unique_ptr<SceneFactory> sceneFactory_{ nullptr };
};