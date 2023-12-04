#include "SceneTransitionManager.h"

void SceneTransitionManager::Update(void)
{
    // シーン遷移を再生する
    if (is_playingAnimation_)
    {
        // フェーズ遷移と更新処理を分けることで、遷移時に更新処理が1フレーム呼ばれないのを防ぐ
        CheckPhase();
        ExecutePhase();
    }
}

void SceneTransitionManager::Draw(void)
{
    // シーン遷移を再生する
    if (is_playingAnimation_)
    {
        // 現在の動作中演出に対応する描画関数
        switch (currentPhase_)
        {
        case TransitionType::FADEIN:
            sceneTransitionPtr_[0]->Display();
            break;
        case TransitionType::MIDDLE:
            sceneTransitionPtr_[1]->Display();
            break;
        case TransitionType::FADEOUT:
            sceneTransitionPtr_[2]->Display();
            break;
        default:
            break;
        }
    }
}

void SceneTransitionManager::PlaySceneTransition(bool arg_isStopMiddle)
{
    for (size_t i = 0; i < 3; i++)
    {
        // シーン遷移演出が格納された変数をまとめて初期化。
        sceneTransitionPtr_[i].reset();
        // シーン遷移演出を生成
        const auto name = (SceneTransitionName)i;
        sceneTransitionPtr_[i] = sceneTransitionFactory_.Create(name);
    }

    // シーン遷移演出の再生開始
    sceneTransitionPtr_[0]->Start();

    // 再生状態を初期化
    currentPhase_ = TransitionType::FADEIN;
    // 再生中フラグを true
    is_playingAnimation_ = true;
    is_stopMiddle_ = arg_isStopMiddle;
}

void SceneTransitionManager::StopSceneTransition(void)
{
    is_playingAnimation_ = false;
}

void SceneTransitionManager::ResumeSceneTransition(void)
{
    // 止まらない設定の場合、不要なのでスキップ
    if (is_stopMiddle_ == false) { return; }

    // 遷移フェーズがMIDDLE以外ならスキップ
    if (currentPhase_ != TransitionType::MIDDLE) { return; }
    // MIDDLEのタイマーが完遂していないならスキップ
    //if (sceneTransitionPtr_[1]->GetTimer().GetIsFinished() == false) { return; }

    // 遷移フェーズをFADEOUTへ変更
    currentPhase_ = TransitionType::FADEOUT;
    // シーン遷移演出の停止
    sceneTransitionPtr_[1]->Finish();
    // 次のシーン遷移演出の再生開始
    sceneTransitionPtr_[2]->Start();
}

void SceneTransitionManager::CheckPhase(void)
{
    // フェーズに応じた、遷移の必要性があるかの確認
    switch (currentPhase_)
    {
    case TransitionType::FADEIN:
        // 終了している場合
        if (sceneTransitionPtr_[0]->GetTimer().GetIsFinished())
        {
            // 遷移フェーズを変更
            currentPhase_ = TransitionType::MIDDLE;
            // シーン遷移演出の停止
            sceneTransitionPtr_[0]->Finish();
            // 次のシーン遷移演出の再生開始
            sceneTransitionPtr_[1]->Start();
        }
        break;

    case TransitionType::MIDDLE:
        // trueなら、シーンフェーズ移行処理をスキップする。
        if (is_stopMiddle_) { break; }

        // 遷移フェーズをFADEOUTへ変更
        currentPhase_ = TransitionType::FADEOUT;
        // シーン遷移演出の停止
        sceneTransitionPtr_[1]->Finish();
        // 次のシーン遷移演出の再生開始
        sceneTransitionPtr_[2]->Start();

        // タイマーの終了処理と遷移フェーズの変更は、ResumeSceneTransition()で行う
        break;

    case TransitionType::FADEOUT:
        // 終了している場合
        if (sceneTransitionPtr_[2]->GetTimer().GetIsFinished())
        {
            // シーン遷移終了
            StopSceneTransition();
            // シーン遷移演出の停止
            sceneTransitionPtr_[2]->Finish();
        }
        break;

    default:
        break;
    }
}

void SceneTransitionManager::ExecutePhase(void)
{
    // 現在の動作中演出に対応する更新関数
    switch (currentPhase_)
    {
    case TransitionType::FADEIN:
        // 更新処理
        sceneTransitionPtr_[0]->Execute();
        break;

    case TransitionType::MIDDLE:
        // 更新処理
        sceneTransitionPtr_[1]->Execute();
        break;

    case TransitionType::FADEOUT:
        // 更新処理
        sceneTransitionPtr_[2]->Execute();
        break;

    default:
        break;
    }
}
