#include "SceneTransition.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

void SceneTransition_FadeInOut::Initialize(void)
{
    png_deepBlue_.SetColor({ 0,0,0,1 });
}

void SceneTransition_FadeInOut::Execute(void)
{
    // 再生中である
    if (timer_.GetIsFinished() == false)
    {
        png_deepBlue_.Update();
        float alpha = timer_.GetTimeRate();
        png_deepBlue_.SetColor({ 0,0,0,alpha });

        GUI::Begin("SceneTransition_FadeInOut");
        GUI::Text("alpha:%f", alpha);
        GUI::End();
    }

    // タイマーの更新
    timer_.Update();
}

void SceneTransition_FadeInOut::Display(void)
{
    png_deepBlue_.Draw();
}

void SceneTransition_FadeInOut::Start(void)
{
    Initialize();
    timer_.Start(100);
}
