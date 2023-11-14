#pragma once
#include <memory>
#include "SceneNameList.h"

#include "DemoScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

class SceneFactory
{
public:
    //>> 関数
    SceneFactory(void) = default;
    ~SceneFactory(void) = default;

    std::unique_ptr<IScene> CreateScene(SceneName scene);
};
