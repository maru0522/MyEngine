#pragma once
#include <memory>
#include "SceneTransition.h"
#include "SceneTransitionFactory.h"

class SceneTransitionManager
{
public:
    //>> 関数
    SceneTransitionManager(void) = default;
    ~SceneTransitionManager(void) = default;

    void Update(void);
    void Draw(void);

    void PlaySceneTransition(SceneTransitionName arg_sceneName);
    void StopSceneTransition(void);

private:
    //>> 変数
    bool is_playingAnimation_;

    std::unique_ptr<ISceneTransition> sceneTransitionPtr_;
    SceneTransitionFactory sceneTransitionFactory_;

public:
    //>> setter

    //>> getter
    bool IsPlayingAnimation(void) { return is_playingAnimation_; }
};

