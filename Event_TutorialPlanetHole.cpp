#include "Event_TutorialPlanetHole.h"
#include "CameraManager.h"

Event_TutorialPlanetHole::Event_TutorialPlanetHole(CollisionManager* arg_colMPtr) : colMPtr_(arg_colMPtr)
{
    camera_leave_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_leave");
    camera_wait_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_wait");
    camera_approach_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_approach");
    CameraManager::GetInstance()->Register(camera_leave_.get());
    CameraManager::GetInstance()->Register(camera_wait_.get());
    CameraManager::GetInstance()->Register(camera_approach_.get());
    Transform transform(Vector3{ 0.f,53.f,-50.f }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    camera_leave_->SetTransform(transform);

    cameraState_ = CameraState::LEAVE;

    // マネージャーに登録
    colMPtr_->Register(&entrances_[0]);
    colMPtr_->Register(&entrances_[1]);
    // コライダーの名前を設定
    entrances_[0].SetID("tutorialPlanet_hole1_");
    entrances_[1].SetID("tutorialPlanet_hole2_");
    // OnTriggerの設定
    entrances_[0].callback_onCollision_ = std::bind(&Event_TutorialPlanetHole::OnTrigger, this);
    entrances_[1].callback_onCollision_ = std::bind(&Event_TutorialPlanetHole::OnTrigger, this);
    // 各コライダーの半径を設定
    entrances_[0].radius = 1.f * kScaleEntranceSphere;
    //entrances_[0].centers = 1.f * kScaleEntranceSphere;
}

Event_TutorialPlanetHole::~Event_TutorialPlanetHole(void)
{
    CameraManager::GetInstance()->UnRegister(camera_leave_.get());
    CameraManager::GetInstance()->UnRegister(camera_wait_.get());
    CameraManager::GetInstance()->UnRegister(camera_approach_.get());
}

void Event_TutorialPlanetHole::Execute(void)
{
}

void Event_TutorialPlanetHole::Draw(void)
{
}

void Event_TutorialPlanetHole::Initialize(void)
{
    CameraManager::GetInstance()->SetCurrentCamera(camera_leave_.get());
    timer_leaveCam_.Start(kLeaveTimer_);
    timer_leaveCam_.SetAddSpeed(kLeaveAddSpeed_);
}

void Event_TutorialPlanetHole::OnTrigger(void)
{
}

void Event_TutorialPlanetHole::SetIsExecute(bool arg_isExecute)
{
    is_execute_ = arg_isExecute;
    if (arg_isExecute) { Initialize(); }
}
