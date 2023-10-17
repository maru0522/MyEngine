#include "SceneFactory.h"

std::unique_ptr<IScene> SceneFactory::CreateScene(SceneName scene)
{
    // シーンがDEMOなら
    if (scene == SceneName::DEMO) { return std::make_unique<DemoScene>(); }
    // シーンがTITLEなら
    if (scene == SceneName::TITLE) { return std::make_unique<TitleScene>(); }
    // シーンがGAMEなら
    if (scene == SceneName::GAME) { return std::make_unique<GameScene>(); }
    // シーンがRESULTなら
    if (scene == SceneName::RESULT) { return std::make_unique<ResultScene>(); }

    // 何も指定がない場合DEMOシーンを返す
    return std::make_unique<DemoScene>();
}
