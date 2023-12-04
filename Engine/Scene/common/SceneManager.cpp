#include "SceneManager.h"

SceneManager::SceneManager(void)
{
    next_SceneName_ = SceneName::NONE;
}

SceneManager::~SceneManager(void)
{
    if (currentScene_) {
        currentScene_->Finalize();
        currentScene_.reset();
    }
}

SceneManager* SceneManager::GetInstance(void)
{
    static SceneManager instatnce;
    return &instatnce;
}

void SceneManager::RequestChangeScene(SceneName arg_nextScene)
{
    // 次のシーン名を設定
    next_SceneName_ = arg_nextScene;
    // シーン遷移の再生
    sceneTransitionManager_.PlaySceneTransition(false);
}

void SceneManager::Initialize(SceneName firstScene)
{
    // 最初のシーンを生成
    currentScene_ = sceneFactory_.CreateScene(firstScene);
    currentScene_->Initialize();
}

void SceneManager::Update(void)
{
    // シーン遷移が再生中である
    if (sceneTransitionManager_.IsPlayingAnimation())
    {
        // シーン遷移の更新処理
        sceneTransitionManager_.Update();

        // シーン変更が求められている場合 && シーンを変更してよいタイミングの場合
        if (IsNeededSceneChange() && sceneTransitionManager_.IsChngeableScene())
        {
            // シーンを変更
            ChangeScene();
            // 現在シーンの更新処理を1回だけ呼ぶ
            currentScene_->Update();
            // ローディングを開始
            is_lodingData_ = true;
        }
    }
    else
    {
        // 現在シーンの更新処理
        currentScene_->Update();
    }

    // データの読み込み可能
    if (is_lodingData_)
    {
        // データの読み込み処理と完了
        //if ~~
        is_lodingData_ = false;
        sceneTransitionManager_.ResumeSceneTransition();
    }
}

void SceneManager::Draw3d(void)
{
    currentScene_->Draw3d();
}

void SceneManager::Draw2dFore(void)
{
    currentScene_->Draw2dFore();
    if (sceneTransitionManager_.IsPlayingAnimation())
    {
        // シーン遷移の描画
        sceneTransitionManager_.Draw();
    }
}

void SceneManager::Draw2dBack(void)
{
    currentScene_->Draw2dBack();
}

bool SceneManager::IsNeededSceneChange(void)
{
    // 次のシーン名が "NONE" 以外である
    if (next_SceneName_ != SceneName::NONE) { return true; }

    return false;
}

void SceneManager::ChangeScene(void)
{
    // nullチェック
    if (currentScene_)
    {
        // 現在シーンの終了処理
        currentScene_->Finalize();
        // ユニークポインタの初期化
        currentScene_.reset();

        // シーン移行
        currentScene_ = sceneFactory_.CreateScene(next_SceneName_);
        // 次シーンの指定を初期化
        next_SceneName_ = SceneName::NONE;
        // 現在シーンの初期化処理
        currentScene_->Initialize();
    }
}
