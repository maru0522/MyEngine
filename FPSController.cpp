#include "FPSController.h"
#include <thread>

void FPSController::Initialize(void)
{
    // ���ݎ��Ԃ��L�^����
    timeRef_ = std::chrono::steady_clock::now();
}

void FPSController::Update(void)
{
    const std::chrono::microseconds kMinTime_{ uint64_t(1000000.0f / fps_) }; // 1/60�b�҂�����̎���
    const std::chrono::microseconds kMinCheckTime_{ uint64_t(1000000.0f / (fps_ + 5)) }; // 1/60�b���͂��ɒZ������

    //�@���ݎ��Ԃ��擾����
    std::chrono::steady_clock::time_point now{ std::chrono::steady_clock::now() };
    // �O��L�^����̌o�ߎ��Ԃ��擾����
    std::chrono::microseconds elapsed{ std::chrono::duration_cast<std::chrono::microseconds>(now - timeRef_) };

    // 1/60�b�i���͂��ɒZ�����ԁj�o���Ă��Ȃ��ꍇ
    if (elapsed < kMinCheckTime_) {
        // 1/60�b�o�߂���܂Ŕ����ȃX���[�v���J��Ԃ�
        while (std::chrono::steady_clock::now() - timeRef_ < kMinTime_)
        {
            // 1�}�C�N���b�X���[�v
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }



    // ���݂̎��Ԃ��L�^
    timeRef_ = std::chrono::steady_clock::now();
}
