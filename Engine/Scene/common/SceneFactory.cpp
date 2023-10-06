#include "SceneFactory.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(Usage scene)
{
    // シーンがDEMOなら
    if (scene == Usage::DEMO) { return std::make_unique<DemoScene>(); }
    // シーンがTITLEなら
    if (scene == Usage::TITLE) { return std::make_unique<TitleScene>(); }
    // シーンがGAMEなら
    if (scene == Usage::GAME) { return std::make_unique<GameScene>(); }
    // シーンがRESULTなら
    if (scene == Usage::RESULT) { return std::make_unique<ResultScene>(); }

    // 何も指定がない場合DEMOシーンを返す
    return std::make_unique<DemoScene>();
}
