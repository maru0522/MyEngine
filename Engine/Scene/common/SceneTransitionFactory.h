#pragma once
#include "SceneTransition.h"
#include <memory>
/**
 * @file SceneTransitionFactory.h
 * @brief シーン遷移クラスを生成するクラスが宣言されたファイル
 */

enum class SceneTransitionName
{
    NONE = -1,

    FADEIN,
    MIDDLE,
    FADEOUT,
};

class SceneTransitionFactory
{
public:
    //>> 関数
    SceneTransitionFactory(void) = default;
    ~SceneTransitionFactory(void) = default;

    //>> 変数
    std::unique_ptr<ISceneTransition> Create(SceneTransitionName arg_sceneName);
};

