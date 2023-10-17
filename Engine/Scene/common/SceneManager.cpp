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
    next_SceneName_ = arg_nextScene;
}

void SceneManager::Initialize(SceneName firstScene)
{
    // 最初のシーンを生成
    currentScene_ = sceneFactory_.CreateScene(firstScene);
    currentScene_->Initialize();
}

void SceneManager::Update(void)
{
    // シーン遷移再生中ではない
    if (sceneTransitionManager_.IsPlayingAnimation() == false)
    {
        // シーン遷移をする必要がある
        if (IsNeedSceneChange())
        {
            // シーンを変更
            ChangeScene();
            // シーン遷移の再生
            sceneTransitionManager_.PlaySceneTransition(SceneTransitionName::FADEINOUT);

            // シーン遷移マネージャーを更新
            sceneTransitionManager_.Update();
            return;
        }

        // 現在シーンUpdate()
        currentScene_->Update();
    }
    // シーン遷移再生中である
    else
    {
        // シーン遷移マネージャーを更新
        sceneTransitionManager_.Update();
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
