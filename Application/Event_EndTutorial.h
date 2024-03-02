#pragma once
#include "Sprite.h"
#include "NormalCamera.h"
#include "Timer.h"

class Event_EndGame
{
private:
    //>> 定義
    enum class CameraState
    {
        CLOSE,
        WAIT1,
        FINISH,
    };

    const float kCloseTimer_ = 3.f;
    const float kColseAddSpeed_ = 2.f;
    const float kWaitTimer_ = 3.f;
    const float kWaitAddSpeed_ = 1.f;

public:
    enum class DisplayString
    {
        CLEAR,
        GAMEOVER,
        TIMEOVER,
    };

    //>> 関数
    Event_EndGame(void) = default;
    ~Event_EndGame(void) = default;

    void Initialize(CameraManager* arg_cameraMPtr);
    void Execute(void);
    void Draw(void);
    void Finalize(void);

private:
    void Start(void);
    void Update_CloseCam(void);
    void Update_WaitCam(void);

    //>> 変数
    std::array<std::unique_ptr<Sprite>, 2> cinemas_;
    std::array<std::unique_ptr<Sprite>, 2> stringBackGrounds_;
    std::unique_ptr<Sprite> string1_;
    std::unique_ptr<Sprite> string2_;
    std::unique_ptr<Sprite> string3_;

    CameraManager* cameraMPtr_;
    std::unique_ptr<NormalCamera> camera_;
    CameraState cameraState_;

    DeltaTimer timer_closeCam_;
    DeltaTimer timer_waitCam_;

    bool is_execute_;
    DisplayString displayString_;

public:
    //>> setter
    void SetIsExecute(bool arg_isExecute, DisplayString arg_displayString);

    //>> getter
    bool GetIsExecite(void) { return is_execute_; }
};



