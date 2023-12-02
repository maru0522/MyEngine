#pragma once
#include <chrono>

class ITimer
{
public:
    //>> 定義
    using nanoseconds = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;
    using minutes = std::chrono::minutes;
    using hours = std::chrono::hours;

    //>> 関数
    ITimer(void) = default;
    virtual ~ITimer(void) = default;

    /// <summary>
    /// <para>Windowsが起動してからの経過時間をミリ秒単位であらわした値を返す。</para>
    /// <para>日時等を取得するわけではないが、この関数の存在意義は時間計測にある。</para>
    /// <para>TimeUnitはstd::chrono::durationの別名にある[時間間隔を表す型]</para>
    /// <para>のみを使用できる。</para>
    /// </summary>
    /// <typeparam name="TimeUnit"></typeparam>
    /// <returns></returns>
    template<class TimeUnit>
    static int32_t GetNowCount(void) {
        return (int32_t)duration_cast<TimeUnit>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    /// <summary>
    /// <para>帰ってから書きゃええ</para>
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    static std::string GetNowDateTime(void) {
        std::time_t now = std::time(nullptr);
        std::tm tm{};
        localtime_s(&tm, &now);

        std::stringstream ss;
        ss << 1900 + tm.tm_year << ":" << std::setfill('0') << std::setw(2) << tm.tm_mon + 1 << ":" << tm.tm_mday << " "
            << std::setfill('0') << std::setw(2) << tm.tm_hour << ":" << std::setfill('0') << std::setw(2) << tm.tm_min
            << ":" << std::setfill('0') << std::setw(2) << tm.tm_sec;
        return ss.str();
    }

protected:
    //>> 変数
    float gameSpeed_{ 1.f };

public:
    //>> setter
    void SetGameSpeed(float gameSpeed) { gameSpeed_ = gameSpeed; }

    //>> getter
    float GetGameSpeed(void) { return gameSpeed_; }
};


class InternalTimer final : public ITimer
{
public:
    //>> 関数
    InternalTimer(float sec_finishTime = 1.f) : sec_finishTime_(sec_finishTime) {}
    InternalTimer(void) : InternalTimer(0.f) {}
    ~InternalTimer(void) override = default;

    // 開始
    void Start(void);
    void Start(float sec_finishTime = 1.0f);
    // 停止 
    void Pause(void); // int32_tを使っている為、596時間程しか一時停止できない。
    // 再開
    void Resume(void);
    // 終了
    void Finish(void);

    // スローモーション
    void StartSlow(float spd = 1.f);
    void FinishSlow(void);

private:
    //>> 変数
    int32_t mil_startTime_{};     // 計測開始時のPC内部の時間
    int32_t mil_pauseTime_{};     // 一時停止時のPC内部の時間
    int32_t mil_slowTime_{};      // スローモーション開始時のPC内部の時間
    float mil_totalPuaseTime_{};  // 停止していた時間が合計何ミリ秒か
    float sec_finishTime_{};      // ゴールを何秒にするか

public:
    //>> setter
    void SetEndTime(float finishTime) { sec_finishTime_ = finishTime; }

    //>> getter
    const float GetElapsedTime(); // 経過時間取得
    bool GetIsSlow(void) { return mil_slowTime_ > 0; } // 今スローモーション中か
    const float GetEndTime(void) { return sec_finishTime_; } // 目標時間が何秒か
    const bool GetIsFinished(void) { return sec_finishTime_ <= ((GetNowCount<milliseconds>() - mil_startTime_) / 1000.0f); } // 目的として設定した時間を超えているか（あるいは終了させられたか）
};


class FrameTimer final : public ITimer
{
public:
    //>> 関数
    FrameTimer(int32_t frame_max, int32_t value_add = 1);
    FrameTimer(void) : FrameTimer(100) {}
    ~FrameTimer(void) override = default;

    // 開始
    void Start(void);
    void Start(int32_t frame_max);
    // 更新
    void Update(void);
    // 停止
    void Pause(void);
    // 再開
    void Resume(void);
    // 終了
    void Finish(void);

private:
    //>> 変数
    float frame_current_{}; // 現在値
    int32_t frame_max_{};     // 最大値
    int32_t value_add_{};     // 毎フレーム、幾つ加算するか
    bool is_execute_{};       // 起動の有無
    bool is_pause_{};         // ポーズ中か否か
    bool is_loop_{};          // ループするか

public:
    //>> setter
    void SetMaxFrame(int32_t frame_max) { frame_max_ = frame_max; }
    void SetValueAdd(int32_t value_add) { value_add_ = value_add; }
    void SetIsLoop(bool is_loop) { is_loop_ = is_loop; }

    //>> getter
    bool GetIsExecuted(void) { return is_execute_; }
    bool GetIsLoop(void) { return is_loop_; }
    bool GetIsFinished(void);
    bool GetIsPause(void) { return is_pause_; }
    float GetTimeRate(bool is_clamp0To1 = true);
    float GetFrameCurrent(void) { return frame_current_; }
    int32_t GetAddValue(void) { return value_add_; }
    int32_t GetFrameMax(void) { return frame_max_; }
};

class DeltaTimer : public ITimer
{
public:
    //>> 関数
    static float DeltaTime(int32_t arg_past_milliSeconds, int32_t arg_current_milliSeconds); // スネークの後ろがmilliSecondsなのは、あくまで要素としてミリ秒数を指定するため

    DeltaTimer(float arg_max_sec) : sec_max_(arg_max_sec) {}
    DeltaTimer(void) : DeltaTimer(100) {}
    ~DeltaTimer(void) override = default;

    // 開始
    void Start(void);
    void Start(float sec_max);
    // 更新
    void Update(void);
    // 停止
    void Pause(void);
    // 再開
    void Resume(void);
    // 終了
    void Finish(bool arg_isResetCurrent = false);

private:
    //>> 変数
    int32_t milliSec_past_{}; // 前回時のミリ秒数
    float addSpeed_{ 1.f };   // 実際の何倍で進むか
    float sec_current_{};     // 現在秒数
    float sec_max_{};         // 最大秒数
    bool is_pause_{};         // ポーズ中か否か
    bool is_loop_{};          // ループするか

public:
    //>> setter
    void SetMaxFrame(float arg_frameMax) { sec_max_ = arg_frameMax; }
    void SetAddSpeed(float arg_addSpeed) { addSpeed_ = arg_addSpeed; }
    void SetIsLoop(bool arg_isLoop) { is_loop_ = arg_isLoop; }

    //>> getter
    bool GetIsLoop(void) { return is_loop_; }
    bool GetIsFinished(void);
    bool GetIsPause(void) { return is_pause_; }
    float GetTimeRate(bool is_clamp0To1 = true);
    float GetFrameCurrent(void) { return sec_current_; }
    float GetFrameMax(void) { return sec_max_; }
};
