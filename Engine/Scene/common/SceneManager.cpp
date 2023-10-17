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

void SceneManager::RequestChangeScene(SceneName arg_nextScene, int32_t arg_waitFrame)
{
    // 次のシーン名を設定
    next_SceneName_ = arg_nextScene;
    // シーンを変更するまでの時間を設定（arg_waitFrame後、シーンが切り替わる）
    timer_waitChangeScene_.Start(arg_waitFrame);
    // シーン遷移の再生
    sceneTransitionManager_.PlaySceneTransition(SceneTransitionName::FADEINOUT);
}

void SceneManager::Initialize(SceneName firstScene)
{
    // 最初のシーンを生成
    currentScene_ = sceneFactory_.CreateScene(firstScene);
    currentScene_->Initialize();
}

void SceneManager::Update(void)
{
    // 現在シーンの更新処理
    currentScene_->Update();

    // シーン遷移が再生中である
    if (sceneTransitionManager_.IsPlayingAnimation())
    {
        // シーンが変更するまでのカウント
        timer_waitChangeScene_.Update();
        // シーン遷移の更新処理
        sceneTransitionManager_.Update();
    }

    // シーンを変更すべきである && シーン変更までの時間が経過している
    if (IsNeedSceneChange() && timer_waitChangeScene_.GetIsFinished())
    {
        // シーンを変更
        ChangeScene();
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

bool SceneManager::IsNeedSceneChange(void)
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
