#pragma once
#include <chrono>

class FPSController
{
public:
    // �֐�
    void Initialize(void);
    void Update(void);

    inline void SetFPS(uint32_t fps) { fps_ = fps; }

private:
    // �ϐ�
    uint32_t fps_{ 60 };

    std::chrono::steady_clock::time_point timeRef_; // ���ԋL�^�iFPS�Œ�p�j
};