#pragma once
#include "Sprite.h"
#include "NormalCamera.h"
#include "Timer.h"
#include "Player.h"
#include "CameraManager.h"

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
        INTERPOLATE,
        FINISH,
    };

    const float kCloseTimer_ = 3.f;
    const float kColseAddSpeed_ = 1.5f;
    const float kWaitTimer_ = 2.f;
    const float kWaitAddSpeed_ = 1.f;
    const float kLeaveTimer_ = 4.f;
    const float kLeaveAddSpeed_ = 1.3f;
    const float kWait2Timer_ = 1.3f;
    const float kWait2AddSpeed_ = 1.f;
    const float kInterpolate_posTimer_ = 2.6f;
    const float kInterpolate_posAddSpeed_ = 1.f;
    const float kInterpolate_axesTimer_ = 4.6f;
    const float kInterpolate_axesAddSpeed_ = 1.f;

public:
    //>> 関数
    Event_StartTutorial(void) = default;
    ~Event_StartTutorial(void) = default;

    void Initialize(CameraManager* arg_cameraMPtr, Player* arg_playerPtr);
    void Execute(void);
    void Draw(void);
    void Finalize(void);

private:
    void Start(void);
    void Update_CloseCam(void);
    void Update_WaitCam(void);
    void Update_LeaveCam(void);
    void Update_WaitCam2(void);
    void Update_Interpolate(void);

    //>> 変数
    std::array<std::unique_ptr<Sprite>,2> cinemas_;
    std::array<std::unique_ptr<Sprite>,2> stringBackGrounds_;
    std::unique_ptr<Sprite> string_;
    std::unique_ptr<NormalCamera> camera_;
    CameraState cameraState_;

    DeltaTimer timer_closeCam_;
    DeltaTimer timer_waitCam_;
    DeltaTimer timer_leaveCam_;
    DeltaTimer timer_waitCam2_;
    DeltaTimer timer_interpolate_pos_;
    DeltaTimer timer_interpolate_axes_;

    Vector3 pos_interpolate_start_;
    Vector3 pos_interpolate_end_;
    Axis3 axes_interpolate_start_;
    Axis3 axes_interpolate_end_;

    Player* playerPtr_;
    CameraManager* cameraMPtr_;

    bool is_execute_;
    bool is_skip_debug_ = true;

public:
    //>> setter
    void SetIsExecute(bool arg_isExecute);

    //>> getter
    bool GetIsExecite(void) { return is_execute_; }
};

