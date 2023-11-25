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
    };

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

    //>> 変数
    std::array<std::unique_ptr<Sprite>,2> cinemas_;
    std::unique_ptr<NormalCamera> camera_;
    CameraState cameraState_;

    DeltaTimer timer_closeCam_;
    DeltaTimer timer_waitCam_;
    DeltaTimer timer_leaveCam_;
};

