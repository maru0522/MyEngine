#include "Timer.h"
#include "Util.h"

void InternalTimer::Start(void)
{
    mil_startTime_ = GetNowCount<milliseconds>();
}

void InternalTimer::Start(float sec_finishTime)
{
    // 既に設定された目的時間を過ぎている（あるいはあるいは終了させられた)
    if (GetIsFinished()) { Finish(); } // 各変数初期化
    // まだ目的時間を過ぎていない && 終了を実行していない状態なら棄却
    else if (mil_startTime_ > 0) return;

    mil_startTime_ = GetNowCount<milliseconds>();
    sec_finishTime_ = sec_finishTime;
}

void InternalTimer::Pause(void)
{
    // 再開を実行していない状態なら棄却
    if (mil_pauseTime_ > 0) return;

    mil_pauseTime_ = GetNowCount<milliseconds>();
}

void InternalTimer::Resume(void)
{
    // 停止を実行していない状態なら棄却
    if (mil_pauseTime_ == 0) return;

    // 停止していた時間の合計 += 今の時間 - 停止した時の時間
    mil_totalPuaseTime_ += GetNowCount<milliseconds>() - mil_pauseTime_;
    // 停止していた時の時間を初期化
    mil_pauseTime_ = 0;
}

void InternalTimer::Finish(void)
{
    // 各変数の初期化
    mil_startTime_ = 0;
    mil_pauseTime_ = 0;
    mil_totalPuaseTime_ = 0.f;
    sec_finishTime_ = 0.f;
    gameSpeed_ = 1.f;
    mil_slowTime_ = 0;
}

void InternalTimer::StartSlow(float spd)
{
    mil_slowTime_ = GetNowCount<milliseconds>();
    gameSpeed_ = spd;
}

void InternalTimer::FinishSlow(void)
{
    // スローモーションを開始していないなら棄却。
    if (mil_slowTime_ == 0) return;

    // 停止していた時間の合計 += (今の時間 - スローモーション開始時の時間) * (1 - 速度)　※速度が0.1で実時間10秒進めた場合、内部では1秒しか立っていないことを表すため、停止していた時間の合計に9秒分足している（後で合計の経過時間から引くため）
    mil_totalPuaseTime_ += (GetNowCount<milliseconds>() - mil_slowTime_) * (1 - gameSpeed_);
    // 停止していた時の時間を初期化
    mil_slowTime_ = 0;
    gameSpeed_ = 1.f;
}

const float InternalTimer::GetElapsedTime()
{
    int32_t current = GetNowCount<milliseconds>();
    int32_t elapsed_pause{};
    int32_t elapsed_slow{};

    if (mil_pauseTime_) elapsed_pause = (current - mil_pauseTime_);
    if (mil_slowTime_) elapsed_slow = uint32_t((current - mil_slowTime_) * (1 - gameSpeed_));

    return float{ (current - mil_startTime_ - mil_totalPuaseTime_ - elapsed_pause - elapsed_slow) / 1000.0f };
}

FrameTimer::FrameTimer(int32_t frame_max, int32_t value_add) : frame_max_(frame_max), value_add_(value_add)
{
}

void FrameTimer::Start(void)
{
    // 最大値が0なら
    if (frame_max_ == 0)
    {
        // ログ出して棄却
        Util::Log::PrintOutputWindow("[FrameTimer]: \"Start()\" could not be executed because frame_max_ is 0.");
        return;
    }

    // 既にタイマーが終了していたら
    if (GetIsFinished())
    {
        // 値を初期化
        frame_current_ = 0;
    }

    is_execute_ = true;
}

void FrameTimer::Start(int32_t frame_max)
{
    frame_max_ = frame_max + 1;
    Start();
}

void FrameTimer::Update(void)
{
    // フレームのカウントが始まっている && ポーズ中ではない
    if (is_execute_ && (is_pause_ == false))
    {
        // フレームのカウントが最大値以上である 
        if (frame_current_ >= frame_max_)
        {
            // ループする
            if (is_loop_)
            {
                // フレームカウントを初期化
                frame_current_ = 0;
            }
            // ループしない
            else
            {
                // 起動フラグを、false
                is_execute_ = false;
            }
        }

        // 現在値 += 加算値 * ゲームスピード
        frame_current_ += value_add_ * gameSpeed_;
    }
}

void FrameTimer::Pause(void)
{
    is_pause_ = true;
}

void FrameTimer::Resume(void)
{
    is_pause_ = false;
}

void FrameTimer::Finish(void)
{
    frame_current_ = (float)frame_max_;
}

bool FrameTimer::GetIsFinished(void)
{
    // ゼロ除算回避
    if (frame_max_ == 0) { return false; }

    return frame_current_ / frame_max_ >= 1.f;
}

float FrameTimer::GetTimeRate(bool is_clamp0To1)
{
    // ゼロ除算回避
    if (frame_max_ == 0) { return 0; }

    // 結果が 0~1 の時clampするか
    if (is_clamp0To1) { return std::clamp(frame_current_ / frame_max_, 0.f, 1.f); }

    return frame_current_ / frame_max_;
}

float DeltaTimer::DeltaTime(int32_t arg_past_milliSeconds, int32_t arg_current_milliSeconds)
{
    // ミリ秒数をint型で取得している前提なので、1000で割って秒数に戻す
    return (arg_current_milliSeconds - arg_past_milliSeconds) / 1000.f;
}

void DeltaTimer::Start(void)
{
    // 最大値が0なら
    if (sec_max_ == 0)
    {
        // ログ出して棄却
        Util::Log::PrintOutputWindow("[FrameTimer]: \"Start()\" could not be executed because frame_max_ is 0.");
        return;
    }

    // 起動済み
    if (is_execute_)
    {
        // ログ出して棄却
        Util::Log::PrintOutputWindow("[FrameTimer]: \"Start()\'' could not be executed because it is already started.");
        return;
    }

    // 起動
    is_execute_ = true;
    // 初期化時点で
    milliSec_past_ = GetNowCount<milliseconds>();
}

void DeltaTimer::Start(float sec_max)
{
    sec_max_ = sec_max;
    Start();
}

void DeltaTimer::Update(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }
    // ポーズ中ならスキップ
    if (is_pause_) { return; }

    // 最大秒数を何秒超えたかの記録用
    float diff{};

    // 秒数カウントが最大秒数を超えているか？
    const bool isOver = sec_current_ >= sec_max_;
    if (isOver)
    {
        // ループフラグ: true
        if (is_loop_)
        {
            // 最大値を何秒超えたかを記録
            diff = sec_current_ - sec_max_;
            // 現在カウントを初期化
            sec_current_ = 0;
        }
        // ループフラグ: false
        else
        {
            // 停止
            Finish();
            return;
        }
    }

    // 現在カウント += 最大値を超えた差分
    sec_current_ += diff;
    // 現在カウント += 加算値 * ゲームスピード
    sec_current_ += DeltaTime(milliSec_past_, GetNowCount<milliseconds>()) * gameSpeed_ * addSpeed_;

    // 前フレームのミリ秒数を記録しておく
    milliSec_past_ = GetNowCount<milliseconds>();
}

void DeltaTimer::Pause(void)
{
    is_pause_ = true;
}

void DeltaTimer::Resume(void)
{
    is_pause_ = false;
}

void DeltaTimer::Finish(bool arg_isResetCurrent)
{
    is_execute_ = false;
    sec_current_ = sec_max_;

    if (arg_isResetCurrent) { sec_current_ = 0.f; }
}

float DeltaTimer::GetTimeRate(bool is_clamp0To1)
{
    // ゼロ除算回避
    if (sec_max_ == 0) { return 1; }

    // 結果が 0~1 の時clampするか
    if (is_clamp0To1) { return std::clamp(sec_current_ / sec_max_, 0.f, 1.f); }

    return sec_current_ / sec_max_;
}
