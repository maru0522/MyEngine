#pragma once
#include <chrono>

class FPSController
{
private:
    // 静的変数
    static constexpr std::chrono::microseconds kMinTime_{ uint64_t(1000000.0f / 60.0f) }; // 1/60秒ぴったりの時間
    static constexpr std::chrono::microseconds kMiCheckTime_{ uint64_t(1000000.0f / 65.0f) }; // 1/60秒より僅かに短い時間

public:
    // 関数
    void Initialize(void);
    void Update(void);

private:
    // 変数
    std::chrono::steady_clock::time_point timeRef_; // 時間記録（FPS固定用）
};