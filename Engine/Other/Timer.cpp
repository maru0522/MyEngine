#include "Timer.h"
#include "Util.h"

void InternalTimer::Start(void)
{
    mil_startTime_ = GetNowCount<milliseconds>();
}

void InternalTimer::Start(float sec_finishTime)
{
    // ���ɐݒ肳�ꂽ�ړI���Ԃ��߂��Ă���i���邢�͂��邢�͏I��������ꂽ)
    if (GetIsFinished()) { Finish(); } // �e�ϐ�������
    // �܂��ړI���Ԃ��߂��Ă��Ȃ� && �I�������s���Ă��Ȃ���ԂȂ���p
    else if (mil_startTime_ > 0) return;

    mil_startTime_ = GetNowCount<milliseconds>();
    sec_finishTime_ = sec_finishTime;
}

void InternalTimer::Pause(void)
{
    // �ĊJ�����s���Ă��Ȃ���ԂȂ���p
    if (mil_pauseTime_ > 0) return;

    mil_pauseTime_ = GetNowCount<milliseconds>();
}

void InternalTimer::Resume(void)
{
    // ��~�����s���Ă��Ȃ���ԂȂ���p
    if (mil_pauseTime_ == 0) return;

    // ��~���Ă������Ԃ̍��v += ���̎��� - ��~�������̎���
    mil_totalPuaseTime_ += GetNowCount<milliseconds>() - mil_pauseTime_;
    // ��~���Ă������̎��Ԃ�������
    mil_pauseTime_ = 0;
}

void InternalTimer::Finish(void)
{
    // �e�ϐ��̏�����
    mil_startTime_ = 0;
    mil_pauseTime_ = 0;
    mil_totalPuaseTime_ = 0.f;
    sec_finishTime_ = 0.f;
    spd_slow_ = 1.f;
    mil_slowTime_ = 0;
}

void InternalTimer::StartSlow(float spd)
{
    mil_slowTime_ = GetNowCount<milliseconds>();
    spd_slow_ = spd;
}

void InternalTimer::FinishSlow(void)
{
    // �X���[���[�V�������J�n���Ă��Ȃ��Ȃ���p�B
    if (mil_slowTime_ == 0) return;

    // ��~���Ă������Ԃ̍��v += (���̎��� - �X���[���[�V�����J�n���̎���) * (1 - ���x)�@�����x��0.1�Ŏ�����10�b�i�߂��ꍇ�A�����ł�1�b���������Ă��Ȃ����Ƃ�\�����߁A��~���Ă������Ԃ̍��v��9�b�������Ă���i��ō��v�̌o�ߎ��Ԃ���������߁j
    mil_totalPuaseTime_ += (GetNowCount<milliseconds>() - mil_slowTime_) * (1 - spd_slow_);
    // ��~���Ă������̎��Ԃ�������
    mil_slowTime_ = 0;
    spd_slow_ = 1.f;
}

const float InternalTimer::GetElapsedTime()
{
    int32_t current = GetNowCount<milliseconds>();
    int32_t elapsed_pause{};
    int32_t elapsed_slow{};

    if (mil_pauseTime_) elapsed_pause = (current - mil_pauseTime_);
    if (mil_slowTime_) elapsed_slow = uint32_t((current - mil_slowTime_) * (1 - spd_slow_));

    return float{ (current - mil_startTime_ - mil_totalPuaseTime_ - elapsed_pause - elapsed_slow) / 1000.0f };
}

FrameTimer::FrameTimer(int32_t frame_max, int32_t value_add) : frame_max_(frame_max), value_add_(value_add)
{
}

void FrameTimer::Start(void)
{
    if (frame_max_ == 0)
    {
        // ���O�o���Ċ��p
        Util::Log::PrintOutputWindow("[FrameTimer]: \"Start()\" could not be executed because frame_max_ is 0.");
        return;
    }

    frame_current_ += value_add_;
}

void FrameTimer::Start(int32_t frame_max)
{
    frame_max_ = frame_max;
    Start();
}

void FrameTimer::Update(void)
{
    // (�t���[���̃J�E���g���n�܂��Ă��� && �ő�t���[���𒴂��Ă��Ȃ�) && �|�[�Y���ł͂Ȃ�
    if (GetIsFinished() && is_pause_ == false)
    {
        if (frame_current_ >= frame_max_ && is_loop_) frame_current_ = 0;

        // ���ݒl += ���Z�l * �Q�[���X�s�[�h
        frame_current_ += value_add_ * value_speed_;
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
    // �[�����Z���
    if (frame_max_ == 0) return false;

    return frame_current_ / frame_max_ >= 1.f;
}

float FrameTimer::GetTimeRate(bool is_clamp0To1)
{
    // �[�����Z���
    if (frame_max_ == 0) return 0;

    // ���ʂ� 0~1 �̎�clamp���邩
    if(is_clamp0To1) return std::clamp(frame_current_ / frame_max_, 0.f, 1.f);

    return frame_current_ / frame_max_;
}
