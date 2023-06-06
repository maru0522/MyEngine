#include "SceneFactory.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(Usage scene)
{
    // シーンがTITLEなら
    if (scene == Usage::DEMO) { return std::make_unique<DemoScene>(); }
    // シーンがTITLEなら
    if (scene == Usage::TITLE) { return std::make_unique<TitleScene>(); }

    // 何も指定がない場合DEMOシーンを返す
    return std::make_unique<DemoScene>();
}
