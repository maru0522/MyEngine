#include "SceneTransitionFactory.h"

std::unique_ptr<ISceneTransition> SceneTransitionFactory::Create(SceneTransitionName arg_sceneName)
{
    if(arg_sceneName == SceneTransitionName::FADEINOUT) { return std::make_unique<SceneTransition_FadeInOut>(); }

    return std::make_unique<SceneTransition_FadeInOut>();
}
