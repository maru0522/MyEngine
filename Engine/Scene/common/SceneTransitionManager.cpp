#include "SceneTransitionManager.h"

void SceneTransitionManager::Update(void)
{
    // シーン遷移が終了している
    if (sceneTransitionPtr_->GetTimer().GetIsFinished()) { StopSceneTransition(); } // is_playingAnimation_ = false

    // シーン遷移を再生する
    if (is_playingAnimation_)
    {
        sceneTransitionPtr_->Execute();
    }
}

void SceneTransitionManager::Draw(void)
{
    // シーン遷移を再生するなら
    if (is_playingAnimation_)
    {
        sceneTransitionPtr_->Display();
    }
}

void SceneTransitionManager::PlaySceneTransition(SceneTransitionName arg_sceneName)
{
    sceneTransitionPtr_.reset();
    sceneTransitionPtr_ = sceneTransitionFactory_.Create(arg_sceneName);
    sceneTransitionPtr_->Start();
    is_playingAnimation_ = true;
}

void SceneTransitionManager::StopSceneTransition(void)
{
    sceneTransitionPtr_->Finish();
    is_playingAnimation_ = false;
}
