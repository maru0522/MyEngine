#pragma once
#include <array>
#include "NormalCamera.h"
#include "Timer.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CameraManager.h"
#include "Player.h"
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
        INTERPOLATION,
        FINISH,
    };

    //const Vector3 kDist_fromPlanetCenter_ = { 43.f,25.5f,9.f };
    const Vector3 kHolePos_relativePlanetCenter = { 41.f,24.f,9.f }; // 穴の判定座標（星の中心点からの相対位置）
    const Vector3 kPlayerPos_Hole0_end = { 44.f,24.f,-10.5f };          // hole0から出てきた時の、着地座標
    const Vector3 kPlayerPos_Hole1_end = { -56.f,-9.f, 5.f };          // hole1から出てきた時の、着地座標

    const Vector3 kCameraPos_leave_start = { 72.f,15.f,-12.f };     // cam_leaveの初期座標（星の中心点からの相対位置）
    const Vector3 kCameraPos_wait = { 0.f,0.f,-210.f };             // cam_waitの座標（星の中心点からの相対位置）
    const Vector3 kCameraPos_approach_end = { -105.f, -26.f, 26.f };    // cam_approachの終了座標（星の中心点からの相対位置）

    const float kLeaveTimer_ = 3.f;
    const float kWaitTimer_ = 5.f;
    const float kApproachTimer_ = 4.f;
    const float kInterpolationTimer_ = 300.f;
    const float kCommonAddSpeed_ = 2.f;

    // 穴の入口にセットするイベントエリアの半径（球状)
    const float kScaleEntranceSphere = 9.8f;

public:
    //>> 関数
    Event_TutorialPlanetHole(CollisionManager* arg_colMPtr,CameraManager* arg_cameraMPtr, Player* arg_playerPtr);
    ~Event_TutorialPlanetHole(void);

    void Execute(void);
    void Draw(void);

private:
    void Initialize(bool arg_isHole0);
    void Update_LeaveCam(void);
    void Update_WaitCam(void);
    void Update_ApproachCam(void);
    void Update_InterpolationCam(void);

    void OnTrigger_Hole0(void);
    void OnTrigger_Hole1(void);

    //>> 変数
    // カメラ達
    std::unique_ptr<NormalCamera> camera_leave_;         // 離れる
    std::unique_ptr<NormalCamera> camera_wait_;          // 中間
    std::unique_ptr<NormalCamera> camera_approach_;      // 近づく
    std::unique_ptr<NormalCamera> camera_interpolation_; // 最後の補間
    // イベントが管理する、カメラの状況
    CameraState cameraState_;

    // タイーマ
    DeltaTimer timer_player_;
    DeltaTimer timer_leaveCam_;
    DeltaTimer timer_waitCam_;
    DeltaTimer timer_approachCam_;
    DeltaTimer timer_interpolation_;

    // イージング用の初期座標
    Vector3 pos_leaveCamStart_;
    Vector3 pos_approachCamEnd_;
    std::vector<Vector3> points_playerSplineHole_;

    // 穴の当たり判定
    std::array<CollisionPrimitive::SphereCollider, 2> entrances_;

    // 星の中心点
    Vector3 planetPos_; // 基本的にこれを基準に当たり判定の位置を決める。
    std::array<std::unique_ptr<Object3D>, 2> sphere_checkColRanges_;

    // コンストラクタで受け取ったptr
    CollisionManager* colMPtr_;
    CameraManager* cameraMPtr_;
    Player* playerPtr_;

    bool is_execute_{};
    bool is_showHoleCollision_{false};
};

