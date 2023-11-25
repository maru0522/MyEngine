#pragma once
#include "Sprite.h"
#include "NormalCamera.h"
#include "Timer.h"

class Event_StartTutorial
{
private:
    //>> 定義
    enum class CameraState
    {
        CLOSE,
        WAIT1,
        LEAVE,
        WAIT2,
        FINISH,
    };

    const float kDef_closeTimer_ = 6.f;
    const float kDef_waitTimer_ = 5.f;
    const float kDef_leaveTimer_ = 8.f;
    const float kDef_wait2Timer_ = 6.f;

public:
    //>> 関数
    Event_StartTutorial(void);
    ~Event_StartTutorial(void);

    void Execute(void);
    void Draw(void);

private:
    void Update_CloseCam(void);
    void Update_WaitCam(void);
    void Update_LeaveCam(void);
    void Update_WaitCam2(void);

    //>> 変数
    std::array<std::unique_ptr<Sprite>,2> cinemas_;
    std::unique_ptr<NormalCamera> camera_;
    CameraState cameraState_;

    DeltaTimer timer_closeCam_;
    DeltaTimer timer_waitCam_;
    DeltaTimer timer_leaveCam_;
    DeltaTimer timer_waitCam2_;
};

