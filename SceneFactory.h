#pragma once
#include <memory>
#include <string>

#include "DemoScene.h"
#include "TitleScene.h"

class SceneFactory
{
public:
    // ��`
    enum class Usage
    {
        DEMO,
        TITLE,
    };

    // �֐�
    std::unique_ptr<IScene> CreateScene(Usage scene);
};
