#pragma once
#include <chrono>
/**
 * @file FPSController.h
 * @brief frame/secondを指定した値以下になるよう調整するクラス。
 */

class FPSController final
{
public:
    // 関数
    void Initialize(void);
    void Update(void);

    inline void SetFPS(uint32_t fps) { fps_ = fps; }

private:
    // 変数
    uint32_t fps_{ 60 };

    std::chrono::steady_clock::time_point timeRef_{}; // 時間記録（FPS固定用）
};
