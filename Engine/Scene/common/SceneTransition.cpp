#include "SceneTransition.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"
#include "WndAPI.h"
//
//void SceneTransition_FadeInOut::Initialize(void)
//{
//    png_deepBlue_.SetColor({ 0,0,0,1 });
//}
//
//void SceneTransition_FadeInOut::Execute(void)
//{
//    // 再生中である
//    if (timer_.GetIsFinished() == false)
//    {
//        // スプライトの更新処理
//        png_deepBlue_.Update();
//
//        // 暗転していない
//        if (is_out_ == false)
//        {
//            // 暗転させる
//            float alpha = Math::Ease::EaseOutSine(timer_.GetTimeRate());
//            png_deepBlue_.SetColor({ 0,0,0,alpha });
//
//            GUI::Begin("SceneTransition_FadeInOut");
//            GUI::Text("FadeOut");
//            GUI::Text("alpha:%f", alpha);
//            GUI::End();
//        }
//        // 暗転している
//        else
//        {
//            // 明転させる
//            float alpha = Math::Ease::EaseOutSine(timer_.GetTimeRate(),1,0);
//            png_deepBlue_.SetColor({ 0,0,0,alpha });
//
//            GUI::Begin("SceneTransition_FadeInOut");
//            GUI::Text("FadeIn");
//            GUI::Text("alpha:%f", alpha);
//            GUI::End();
//        }
//
//    }
//
//    // タイマーの更新
//    timer_.Update();
//
//    // 再生が終了した && 暗転していない
//    if (timer_.GetIsFinished() && is_out_ == false)
//    {
//        is_out_ = true;
//        timer_.Start(50);
//    }
//}
//
//void SceneTransition_FadeInOut::Display(void)
//{
//    png_deepBlue_.Draw();
//}
//
//void SceneTransition_FadeInOut::Start(void)
//{
//    Initialize();
//    timer_.Start(50);
//}
