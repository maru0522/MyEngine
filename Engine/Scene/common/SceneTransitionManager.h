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

    void PlaySceneTransition(bool arg_isStopMiddle = true);
    void StopSceneTransition(void);
    void ResumeSceneTransition(void);

private:
    void CheckPhase(void);
    void ExecutePhase(void);

    //>> 変数
    std::array<std::unique_ptr<ISceneTransition>,3> sceneTransitionPtr_;
    SceneTransitionFactory sceneTransitionFactory_;
    TransitionType currentPhase_;
    bool is_playingAnimation_;
    bool is_stopMiddle_;

public:
    //>> setter

    //>> getter
    bool IsPlayingAnimation(void) { return is_playingAnimation_; }
    bool IsChngeableScene(void) { return currentPhase_ == TransitionType::MIDDLE; }
};

