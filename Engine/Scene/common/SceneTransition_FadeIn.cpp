#include "SceneTransition_FadeIn.h"
#include "MathUtil.h"

SceneTransition_FadeIn::SceneTransition_FadeIn(void)
    : ISceneTransition(TransitionType::FADEIN)
{
}

void SceneTransition_FadeIn::Initialize(void)
{
    // 色
    const Vector4 color = { 0.f,0.f,0.f,0.f };
    png_white1x1_.SetColor(color);

    // サイズ
    const Vector2 size = { WndAPI::kWidth_,WndAPI::kHeight_ };
    png_white1x1_.SetSize(size);
}

void SceneTransition_FadeIn::Execute(void)
{
    // タイマーが終了しているならスキップ
    if (timer_.GetIsFinished()) { return; }

    // スプライトの更新処理
    png_white1x1_.Update();

    // 暗転させる
    const float rate = timer_.GetTimeRate();
    const float alpha = Math::Ease::EaseOutSine(rate);
    const Vector4 color = { 0.f,0.f,0.f,alpha };
    png_white1x1_.SetColor(color);

    // タイマーの更新
    timer_.Update();
}

void SceneTransition_FadeIn::Display(void)
{
    // 描画処理
    png_white1x1_.Draw();
}

void SceneTransition_FadeIn::Start(void)
{
    // 初期化
    Initialize();
    // タイマー起動
    timer_.Start(50);
}
