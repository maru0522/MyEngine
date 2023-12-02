#include "SceneTransition_Middle.h"
#include "MathUtil.h"

SceneTransition_Middle::SceneTransition_Middle(void) 
    : ISceneTransition(TransitionType::MIDDLE)
{
}

void SceneTransition_Middle::Initialize(void)
{
    // 色
    const Vector4 color = { 0.f,0.f,0.f,1.f };
    png_white1x1_.SetColor(color);

    // サイズ
    const Vector2 size = { WndAPI::kWidth_,WndAPI::kHeight_ };
    png_white1x1_.SetSize(size);
}

void SceneTransition_Middle::Execute(void)
{
    // タイマーが終了しているならスキップ
    if (timer_.GetIsFinished()) { return; }

    // スプライトの更新処理
    png_white1x1_.Update();

    // タイマーの更新
    timer_.Update();
}

void SceneTransition_Middle::Display(void)
{
    // 描画処理
    png_white1x1_.Draw();
}

void SceneTransition_Middle::Start()
{
    // 初期化
    Initialize();
    // タイマー起動
    timer_.Start(1);
}
