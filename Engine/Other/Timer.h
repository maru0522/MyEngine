#pragma once
#include <chrono>

class ITimer
{
public:
    // ��`
    using nanoseconds = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;
    using minutes = std::chrono::minutes;
    using hours = std::chrono::hours;

    // �֐�
    ITimer(void) = default;
    virtual ~ITimer(void) = default;

    // �ÓI�֐�
    /// <summary>
    /// <para>Windows���N�����Ă���̌o�ߎ��Ԃ��~���b�P�ʂł���킵���l��Ԃ��B</para>
    /// <para>���������擾����킯�ł͂Ȃ����A���̊֐��̑��݈Ӌ`�͎��Ԍv���ɂ���B</para>
    /// <para>TimeUnit��std::chrono::duration�̕ʖ��ɂ���[���ԊԊu��\���^]</para>
    /// <para>�݂̂��g�p�ł���B</para>
    /// </summary>
    /// <typeparam name="TimeUnit"></typeparam>
    /// <returns></returns>
    template<class TimeUnit>
    static int32_t GetNowCount(void) { 
        return (int32_t)duration_cast<TimeUnit>(std::chrono::steady_clock::now().time_since_epoch()).count(); 
    }

    /// <summary>
    /// <para>�A���Ă��珑���Ⴆ��</para>
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
};


class InternalTimer final : public ITimer
{
public:
    //>> �֐�
    InternalTimer(float sec_finishTime = 1.f) : sec_finishTime_(sec_finishTime) {}
    InternalTimer(void) : InternalTimer(0.f) {}
    virtual ~InternalTimer(void) = default;

    // �J�n
    void Start(void);
    void Start(float sec_finishTime = 1.0f);
    // ��~ 
    void Pause(void); // int32_t���g���Ă���ׁA596���Ԓ������ꎞ��~�ł��Ȃ��B
    // �ĊJ
    void Resume(void);
    // �I��
    void Finish(void);

    // �X���[���[�V����
    void StartSlow(float spd = 1.f);
    void FinishSlow(void);

private:
    //>> �ϐ�
    int32_t mil_startTime_{};     // �v���J�n����PC�����̎���
    int32_t mil_pauseTime_{};     // �ꎞ��~����PC�����̎���
    float mil_totalPuaseTime_{};  // ��~���Ă������Ԃ����v���~���b��
    float sec_finishTime_{};         // �S�[�������b�ɂ��邩

    float spd_slow_{ 1.f };     // �v�����ԂɃX���[���[�V������K�p�ł���B
    int32_t mil_slowTime_{};      // slow�J�n����PC�����̎���

public:
    //>> setter
    void SetEndTime(float finishTime) { sec_finishTime_ = finishTime; }

    //>> getter
    const float GetElapsedTime(); // �o�ߎ��Ԏ擾
    bool GetIsSlow(void) { return mil_slowTime_ > 0; } // ���X���[���[�V��������
    const float GetEndTime(void) { return sec_finishTime_; } // �ڕW���Ԃ����b��
    const bool GetIsFinished(void) { return sec_finishTime_ <= ((GetNowCount<milliseconds>() - mil_startTime_) / 1000.0f); } // �ړI�Ƃ��Đݒ肵�����Ԃ𒴂��Ă��邩�i���邢�͏I��������ꂽ���j
};


class FrameTimer final : public ITimer
{
public:
    //>> �֐�
    FrameTimer(int32_t frame_max,int32_t value_add = 1);
    FrameTimer(void) : FrameTimer(100) {}
    virtual ~FrameTimer(void) = default;

    // �J�n
    void Start(void);
    void Start(int32_t frame_max);
    // �X�V
    void Update(void);
    // ��~
    void Pause(void);
    // �ĊJ
    void Resume(void);
    // �I��
    void Finish(void);

private:
    //>> �ϐ�
    float frame_current_{};   // ���ݒl
    float value_speed_{};     // �Q�[���X�s�[�h
    int32_t frame_max_{};     // �ő�l
    int32_t value_add_{};     // ���t���[���A����Z���邩
    bool is_pause_{};         // �|�[�Y�����ۂ�
    bool is_loop_{};          // ���[�v���邩

public:
    //>> setter
    void SetMaxFrame(int32_t frame_max) { frame_max_ = frame_max; }
    void SetValueAdd(int32_t value_add) { value_add_ = value_add; }
    void SetValueSpeed(float value_speed) { value_speed_ = value_speed; }
    void SetIsLoop(bool is_loop) { is_loop_ = is_loop; }

    //>> getter
    bool GetIsLoop(void) { return is_loop_; }
    bool GetIsFinished(void);
    bool GetIsPause(void) { return is_pause_; }
    float GetTimeRate(bool is_clamp0To1 = true);
    float GetFrameCurrent(void) { return frame_current_; }
    int32_t GetAddValue(void) { return value_add_; }
    int32_t GetFrameMax(void) { return frame_max_; }
};