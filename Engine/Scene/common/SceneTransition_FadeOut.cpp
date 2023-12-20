#include "SceneTransition_FadeOut.h"
#include "MathUtil.h"

SceneTransition_FadeOut::SceneTransition_FadeOut(void)
    : ISceneTransition(TransitionType::FADEOUT)
{
}

void SceneTransition_FadeOut::Initialize(void)
{
    // 色
    const Vector4 color = { 0.f,0.f,0.f,1.f };
    png_white1x1_.SetColor(color);

    // サイズ
    const Vector2 size = { WndAPI::kWidth_,WndAPI::kHeight_ };
    png_white1x1_.SetSize(size);
}

void SceneTransition_FadeOut::Execute(void)
{
    // タイマーが終了しているならスキップ
    if (timer_.GetIsFinished()) { return; }

    // スプライトの更新処理
    png_white1x1_.Update();

    // 明転させる
    const float rate = timer_.GetTimeRate();
    const float alpha = 1.f - Math::Ease::EaseOutSine(rate);
    const Vector4 color = { 0.f,0.f,0.f,alpha };
    png_white1x1_.SetColor(color);

    // タイマーの更新
    timer_.Update();
}

void SceneTransition_FadeOut::Display(void)
{
    // 描画処理
    png_white1x1_.Draw();
}

void SceneTransition_FadeOut::Start()
{
    // 初期化
    Initialize();
    // タイマー起動
    timer_.Start(50);
}
