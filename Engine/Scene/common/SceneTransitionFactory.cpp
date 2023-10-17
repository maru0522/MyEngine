#include "SceneTransitionFactory.h"

std::unique_ptr<ISceneTransition> SceneTransitionFactory::Create(SceneTransitionName arg_sceneName)
{
    if(arg_sceneName == SceneTransitionName::FADEIN) { return std::make_unique<SceneTransition_FadeIn>(); }

    return std::make_unique<SceneTransition_FadeIn>();
}
