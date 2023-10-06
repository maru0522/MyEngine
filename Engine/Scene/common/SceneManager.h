#pragma once
#include <memory>
#include "IScene.h"
#include "SceneFactory.h"

class SceneManager
{
public:
    //>> 関数
    static SceneManager* GetInstance(void);

    void RequestChangeScene(SceneFactory::Usage nextScene, int32_t waitFrame = 0);

    void Initialize(SceneFactory::Usage firstScene = SceneFactory::Usage::DEMO);
    void Update(void);
    void Draw3d(void);
    void Draw2dFore(void);
    void Draw2dBack(void);

private:
    //>> 変数
    int32_t waitFrame_{};
    std::unique_ptr<IScene> currentScene_{ nullptr };
    std::unique_ptr<IScene> nextScene_{ nullptr };

    std::unique_ptr<SceneFactory> sceneFactory_{ nullptr };

    //>> singleton
    SceneManager(void);
    ~SceneManager(void);
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
