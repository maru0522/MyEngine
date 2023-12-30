#pragma once
#include "FrameWork.h"
#include "BackGroundTexture.h"
/**
 * @file GameSystem.h
 * @brief FrameWorkに次いで、ゲームの核となるクラス。アプリケーション特有のマネージャークラスの更新処理はこちらで実行する。
 */

class GameSystem final :
    public FrameWork
{
public:
    // 関数
    GameSystem(void) {};

    void Initialize() override;
    void Update(void) override;
    void Draw(void) override;
    void Finalize(void) override;
};
