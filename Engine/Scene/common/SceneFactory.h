#pragma once
#include <memory>

#include "DemoScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

enum class SceneName
{
    NONE = -1,

    DEMO,
    TITLE,
    GAME,
    RESULT,
};

class SceneFactory
{
public:
    //>> 関数
    SceneFactory(void) = default;
    ~SceneFactory(void) = default;

    std::unique_ptr<IScene> CreateScene(SceneName scene);
};
