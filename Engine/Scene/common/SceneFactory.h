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
    // ��`
    enum class Usage
    {
        DEMO,
        TITLE,
        GAME,
        RESULT,
    };

    // �֐�
    std::unique_ptr<IScene> CreateScene(Usage scene);
};
