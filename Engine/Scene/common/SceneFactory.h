#pragma once
#include <memory>
#include <string>

#include "DemoScene.h"
#include "TitleScene.h"

class SceneFactory
{
public:
    // íËã`
    enum class Usage
    {
        DEMO,
        TITLE,
    };

    // ä÷êî
    std::unique_ptr<IScene> CreateScene(Usage scene);
};
