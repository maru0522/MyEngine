#pragma once
#include <memory>
#include <string>

#include "DemoScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

class SceneFactory
{
public:
    // 定義
    enum class Usage
    {
        DEMO,
        TITLE,
        GAME,
        RESULT,
    };

    // 関数
    std::unique_ptr<IScene> CreateScene(Usage scene);
};
