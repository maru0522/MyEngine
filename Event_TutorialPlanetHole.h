#pragma once
#include <array>
#include "NormalCamera.h"
#include "Timer.h"
#include "SphereCollider.h"
#include "CollisionManager.h"

class Event_TutorialPlanetHole
{
private:
    //>> 定義
    enum class CameraState
    {
        LEAVE,
        WAIT,
        APPROACH,
        FINISH,
    };

    const float kLeaveTimer_ = 3.f;
    const float kLeaveAddSpeed_ = 2.f;
    const float kWaitTimer_ = 3.f;
    const float kWaitAddSpeed_ = 1.f;
    const float kApproachTimer_ = 4.f;
    const float kApproachAddSpeed_ = 1.2f;

    // 穴の入口にセットするイベントエリアの半径（球状)
    const float kScaleEntranceSphere = 1.f;

public:
    //>> 関数
    Event_TutorialPlanetHole(CollisionManager* arg_colMPtr);
    ~Event_TutorialPlanetHole(void);

    void Execute(void);
    void Draw(void);

private:
    void Initialize(void);
    void Update_CloseCam(void);
    void Update_WaitCam(void);
    void Update_LeaveCam(void);
    void Update_WaitCam2(void);

    void OnTrigger(void);

    //>> 変数
    // カメラ達
    std::unique_ptr<NormalCamera> camera_leave_;
    std::unique_ptr<NormalCamera> camera_wait_;
    std::unique_ptr<NormalCamera> camera_approach_;
    // イベントが管理する、カメラの状況
    CameraState cameraState_;

    // タイーマ
    DeltaTimer timer_leaveCam_;
    DeltaTimer timer_waitCam_;
    DeltaTimer timer_approachCam_;

    // 
    CollisionManager* colMPtr_;
    std::array<CollisionPrimitive::SphereCollider, 2> entrances_;

    bool is_execute_;

public:
    //>> setter
    void SetIsExecute(bool arg_isExecute);

    //>> getter
    bool GetIsExecite(void) { return is_execute_; }
};

