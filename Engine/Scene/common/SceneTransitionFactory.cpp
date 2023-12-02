#include "SceneTransitionFactory.h"
#include "SceneTransition_FadeIn.h"
#include "SceneTransition_Middle.h"
#include "SceneTransition_FadeOut.h"

std::unique_ptr<ISceneTransition> SceneTransitionFactory::Create(SceneTransitionName arg_sceneName)
{
    if(arg_sceneName == SceneTransitionName::FADEIN) { return std::make_unique<SceneTransition_FadeIn>(); }
    if(arg_sceneName == SceneTransitionName::MIDDLE) { return std::make_unique<SceneTransition_Middle>(); }
    if(arg_sceneName == SceneTransitionName::FADEOUT) { return std::make_unique<SceneTransition_FadeOut>(); }

    return std::make_unique<SceneTransition_FadeIn>();
}
