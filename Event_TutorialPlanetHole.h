#pragma once
#include <array>
#include "NormalCamera.h"
#include "Timer.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Object3D.h"

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

    //const Vector3 kDist_fromPlanetCenter_ = { 43.f,25.5f,9.f };
    const Vector3 kDist_fromPlanetCenter_ = { 41.f,24.f,9.f };

    const float kLeaveTimer_ = 3.f;
    const float kLeaveAddSpeed_ = 2.f;
    const float kWaitTimer_ = 3.f;
    const float kWaitAddSpeed_ = 1.f;
    const float kApproachTimer_ = 4.f;
    const float kApproachAddSpeed_ = 1.2f;

    // 穴の入口にセットするイベントエリアの半径（球状)
    const float kScaleEntranceSphere = 10.6f;

public:
    //>> 関数
    Event_TutorialPlanetHole(CollisionManager* arg_colMPtr,Vector3* arg_playerPosPtr);
    ~Event_TutorialPlanetHole(void);

    void Execute(void);
    void Draw(void);

private:
    void Initialize(void);
    void Update_LeaveCam(void);
    void Update_WaitCam(void);
    void Update_ApproachCam(void);

    void OnTrigger_Hole0(void);
    void OnTrigger_Hole1(void);

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
    Vector3* playerPosPtr_;

    // 星の中心点
    Vector3 planetPos_; // 基本的にこれを基準に当たり判定の位置を決める。
    std::array<std::unique_ptr<Object3D>, 2> sphere_checkColRanges_;

    bool is_execute_;
    bool is_showHoleCollision_;
};

