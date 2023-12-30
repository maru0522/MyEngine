#pragma once
#include <memory>
#include "IScene.h"
#include "SceneFactory.h"
#include "SceneTransitionManager.h"
#include "Timer.h"
/**
 * @file SceneManager.h
 * @brief シーンを管理するクラスが宣言されたファイル
 */

class SceneManager
{
public:
    //>> 関数
    static SceneManager* GetInstance(void);

    void RequestChangeScene(SceneName arg_nextScene);

    void Initialize(SceneName firstScene = SceneName::DEMO);
    void Update(void);
    void Draw3d(void);
    void Draw2dFore(void);
    void Draw2dBack(void);

private:
    // シーンを変更する必要があるかチェック
    bool IsNeededSceneChange(void);
    // シーンを変更する
    void ChangeScene(void);

    //>> 変数
    std::unique_ptr<IScene> currentScene_{ nullptr };
    SceneName next_SceneName_;

    SceneFactory sceneFactory_;
    SceneTransitionManager sceneTransitionManager_;
    bool is_lodingData_;

    //>> singleton
    SceneManager(void);
    ~SceneManager(void);
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};
