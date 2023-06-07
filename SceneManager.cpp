#include "SceneManager.h"

SceneManager::SceneManager(void)
{
    sceneFactory_ = std::make_unique<SceneFactory>();
}

SceneManager::~SceneManager(void)
{
    if (currentScene_) {
        currentScene_->Finalize();
        currentScene_.reset();
    }
}

void SceneManager::RequestChangeScene(std::unique_ptr<IScene>& nextScene, int32_t waitFrame)
{
    nextScene_ = std::move(nextScene);
    waitFrame_ = waitFrame;
}

void SceneManager::Initialize(SceneFactory::Usage firstScene)
{
    // 最初のシーンを生成
    currentScene_ = sceneFactory_->CreateScene(firstScene);
    currentScene_->Initialize();
}

void SceneManager::Update(void)
{
    // 次シーンの予約がある
    if (nextScene_) {
        // 待機フレーム指定がない && 現在シーンがnullptrではない
        if (waitFrame_ == 0 && currentScene_) {
            currentScene_->Finalize();
            currentScene_.reset();
        }

        // シーン移行
        currentScene_ = std::move(nextScene_); // 管理権限移譲
        nextScene_.reset();                    // 次シーンをnullptrにする
        currentScene_->Initialize();           // 現在シーンを初期化
    }

    // 現在シーンUpdate()
    currentScene_->Update();


    // 待機フレームを減少させる
    waitFrame_--;
    // 待機フレームは0以下にならない
    waitFrame_ = (std::max)(waitFrame_, 0);
}

void SceneManager::Draw3d(void)
{
    currentScene_->Draw3d();
}

void SceneManager::Draw2d(void)
{
    currentScene_->Draw2d();
}
