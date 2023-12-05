#include "Event_TutorialPlanetHole.h"
#include "CameraManager.h"
#include "SimplifyImGui.h"

Event_TutorialPlanetHole::Event_TutorialPlanetHole(CollisionManager* arg_colMPtr, Vector3* arg_playerPosPtr) : colMPtr_(arg_colMPtr), playerPosPtr_(arg_playerPosPtr)
{
    camera_leave_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_leave");
    camera_wait_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_wait");
    camera_approach_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_approach");
    CameraManager::GetInstance()->Register(camera_leave_.get());
    CameraManager::GetInstance()->Register(camera_wait_.get());
    CameraManager::GetInstance()->Register(camera_approach_.get());

    cameraState_ = CameraState::LEAVE;

    // マネージャーに登録
    colMPtr_->Register(&entrances_[0]);
    colMPtr_->Register(&entrances_[1]);
    // コライダーの名前を設定
    entrances_[0].SetID("tutorialPlanet_hole0_");
    entrances_[1].SetID("tutorialPlanet_hole1_");
    // OnTriggerの設定
    entrances_[0].callback_onTrigger_ = std::bind(&Event_TutorialPlanetHole::OnTrigger_Hole0, this);
    entrances_[1].callback_onTrigger_ = std::bind(&Event_TutorialPlanetHole::OnTrigger_Hole1, this);
    // 各コライダーの半径を設定
    entrances_[0].radius = 1.f * kScaleEntranceSphere;
    entrances_[1].radius = 1.f * kScaleEntranceSphere;
    //entrances_[0].centers = 1.f * kScaleEntranceSphere;

    // experiment
    // カメラを指定の座標へセット
    camera_leave_->SetPosition(Vector3{ 0.f,7.f,-210.f });
    camera_leave_->SetTargetPos(*playerPosPtr_);
    is_execute_ = true;
}

Event_TutorialPlanetHole::~Event_TutorialPlanetHole(void)
{
    CameraManager::GetInstance()->UnRegister(camera_leave_.get());
    CameraManager::GetInstance()->UnRegister(camera_wait_.get());
    CameraManager::GetInstance()->UnRegister(camera_approach_.get());
}

void Event_TutorialPlanetHole::Execute(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

    camera_leave_->SetTargetPos(*playerPosPtr_);

    switch (cameraState_)
    {
    case Event_TutorialPlanetHole::CameraState::LEAVE:
        Update_LeaveCam();
        break;

    case Event_TutorialPlanetHole::CameraState::WAIT:
        Update_WaitCam();
        break;

    case Event_TutorialPlanetHole::CameraState::APPROACH:
        Update_ApproachCam();
        break;

    case Event_TutorialPlanetHole::CameraState::FINISH:
        is_execute_ = false;
        //SceneManager::GetInstance()->RequestChangeScene(SceneName::TITLE);
        break;

    default:
        break;
    }
}

void Event_TutorialPlanetHole::Draw(void)
{
}

void Event_TutorialPlanetHole::Initialize(void)
{
    // マネージャーに遠のくカメラをセット
    CameraManager::GetInstance()->SetCurrentCamera(camera_leave_.get());
    // 遠のくカメラのイージング用タイマー開始
    timer_leaveCam_.Start(kLeaveTimer_);
    // タイマーの加算量
    timer_leaveCam_.SetAddSpeed(kLeaveAddSpeed_);
}

void Event_TutorialPlanetHole::Update_LeaveCam(void)
{
}

void Event_TutorialPlanetHole::Update_WaitCam(void)
{
}

void Event_TutorialPlanetHole::Update_ApproachCam(void)
{
}

void Event_TutorialPlanetHole::OnTrigger_Hole0(void)
{
    // nullチェック
    if (!entrances_[0].GetOther()) { return; }

    // プレイヤーと触れた場合
    if (entrances_[0].GetOther()->GetID() == "player")
    {
        // イベントを起動
        is_execute_ = true;
        // 共通初期化処理
        Initialize();

        // カメラを指定の座標へセット（Hole0とHole1で座標は違う。）
        Transform transform(Vector3{ 0.f,7.f,-210.f }, Vector3{ 2.32129f,-1.897498f,0.f }, Vector3{ 1.f,1.f,1.f });
        camera_wait_->SetTransform(transform);
    }
}

void Event_TutorialPlanetHole::OnTrigger_Hole1(void)
{
    // nullチェック
    if (!entrances_[1].GetOther()) { return; }

    if (entrances_[1].GetOther()->GetID() == "player")
    {
        // イベントを起動
        is_execute_ = true;
        // 共通初期化処理
        Initialize();

        // カメラを指定の座標へセット（Hole0とHole1で座標は違う。）
        Transform transform(Vector3{ 0.f,53.f,-50.f }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
        camera_wait_->SetTransform(transform);
    }
}
