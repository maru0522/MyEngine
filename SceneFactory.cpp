#include "SceneFactory.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(Usage scene)
{
    // �V�[����TITLE�Ȃ�
    if (scene == Usage::DEMO) { return std::make_unique<DemoScene>(); }
    // �V�[����TITLE�Ȃ�
    if (scene == Usage::TITLE) { return std::make_unique<TitleScene>(); }

    // �����w�肪�Ȃ��ꍇDEMO�V�[����Ԃ�
    return std::make_unique<DemoScene>();
}
