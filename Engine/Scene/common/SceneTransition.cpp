#include "SceneTransition.h"

void SceneTransition_FadeIn::Execute(void)
{
    // 再生中である
    if (timer_.GetIsFinished() == false)
    {

    }

    // タイマーの更新
    timer_.Update();
}

void SceneTransition_FadeIn::Display(void)
{
}
