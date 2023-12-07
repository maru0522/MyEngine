#include "Event_TutorialPlanetHole.h"
#include "CameraManager.h"
#include "SimplifyImGui.h"
#include "MathUtil.h"

Event_TutorialPlanetHole::Event_TutorialPlanetHole(CollisionManager* arg_colMPtr, CameraManager* arg_cameraMPtr, Player* arg_playerPtr)
    : colMPtr_(arg_colMPtr), cameraMPtr_(arg_cameraMPtr), playerPtr_(arg_playerPtr)
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
    entrances_[0].radius = kScaleEntranceSphere;
    entrances_[1].radius = kScaleEntranceSphere;
    // 座標
    entrances_[0].center = planetPos_ + kHolePos_relativePlanetCenter;
    entrances_[1].center = planetPos_ - kHolePos_relativePlanetCenter;
    //entrances_[0].centers = 1.f * kScaleEntranceSphere;

    for (auto& sphere : sphere_checkColRanges_)
    {
        sphere = std::make_unique<Object3D>("Resources/model/sphere/sphere.obj");
    }
}

Event_TutorialPlanetHole::~Event_TutorialPlanetHole(void)
{
    CameraManager::GetInstance()->UnRegister(camera_leave_.get());
    CameraManager::GetInstance()->UnRegister(camera_wait_.get());
    CameraManager::GetInstance()->UnRegister(camera_approach_.get());
    colMPtr_->UnRegister(&entrances_[0]);
    colMPtr_->UnRegister(&entrances_[1]);
}

void Event_TutorialPlanetHole::Execute(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

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

    if (timer_player_.GetTimeRate() > 1.f) { return; }
    // プレイヤーの座標を、穴を通り抜けるように更新し続ける。
    timer_player_.Update();
    const float rate_player = timer_player_.GetTimeRate();
    Vector3 pos_player{};
    pos_player.x = Math::Ease::EaseInOutSine(rate_player, pos_playerStart_.x, pos_playerEnd_.x);
    pos_player.y = Math::Ease::EaseInOutSine(rate_player, pos_playerStart_.y, pos_playerEnd_.y);
    pos_player.z = Math::Ease::EaseInOutSine(rate_player, pos_playerStart_.z, pos_playerEnd_.z);
    playerPtr_->SetPosition(pos_player);


    // 穴の判定可視化していないならスキップ
    if (is_showHoleCollision_ == false) { return; }

    Transform trans;
    trans.position = planetPos_ + kHolePos_relativePlanetCenter;
    trans.rotation = { 0.f,0.f,0.f };
    trans.scale = { kScaleEntranceSphere,kScaleEntranceSphere,kScaleEntranceSphere };

    // 座標 = 星の中心点 + 距離
    sphere_checkColRanges_[0]->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(trans);
    sphere_checkColRanges_[0]->Update();

    // 座標 = 星の中心点 - 距離
    trans.position = planetPos_ - kHolePos_relativePlanetCenter;
    sphere_checkColRanges_[1]->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(trans);
    sphere_checkColRanges_[1]->Update();
}

void Event_TutorialPlanetHole::Draw(void)
{
    // 穴の判定可視化していないならスキップ
    if (is_showHoleCollision_ == false) { return; }

    for (auto& sphere : sphere_checkColRanges_)
    {
        sphere->Draw("Resources/red1x1.png");
    }
}

void Event_TutorialPlanetHole::Initialize(bool arg_isHole0)
{
    // イベントを起動
    is_execute_ = true;
    // プレイヤー用タイマーを起動
    timer_player_.Start(kLeaveTimer_ + kWaitTimer_ + kApproachTimer_);
    timer_player_.Start(kCommonAddSpeed_);
    // プレイヤーのeventStateを設定
    playerPtr_->SetEventState(Player::EventState::PLANET_HOLE);

    // マネージャーに遠のくカメラをセット
    CameraManager::GetInstance()->SetCurrentCamera(camera_leave_.get());
    // 遠のくカメラのイージング用タイマー開始
    timer_leaveCam_.Start(kLeaveTimer_);
    // タイマーの加算量
    timer_leaveCam_.SetAddSpeed(kCommonAddSpeed_);

    // カメラの最初の座標をプレイヤーのカメラの座標に合わせる。
    if (arg_isHole0)
    {
        // camera_leaveを指定の座標へセット（ Hole0用 )
        pos_leaveCamStart_ = planetPos_ + kCameraPos_leave_start;
        // camera_waitを指定の座標へセット（ Hole0用 ）
        const Vector3 pos_camWait = planetPos_ + kCameraPos_wait;
        camera_wait_->SetPosition(pos_camWait);
        
        // 穴に入って反対側まで行けるよう、スタート地点とゴール地点を設定
        pos_playerStart_ = planetPos_ + kHolePos_relativePlanetCenter;
        pos_playerEnd_ = planetPos_ - kHolePos_relativePlanetCenter;
    }
    else
    {
        // camera_leaveを指定の座標へセット（ Hole0用 )
        pos_leaveCamStart_ = planetPos_ - kCameraPos_leave_start;
        // カメラを指定の座標へセット（ Hole1用 ）
        const Vector3 pos = planetPos_ - kCameraPos_wait;
        camera_wait_->SetPosition(pos);
        //camera_wait_->SetTargetPos(*playerPosPtr_);

        // 穴に入って反対側まで行けるよう、スタート地点とゴール地点を設定
        pos_playerStart_ = planetPos_ - kHolePos_relativePlanetCenter;
        pos_playerEnd_ = planetPos_ + kHolePos_relativePlanetCenter;
    }
}

void Event_TutorialPlanetHole::Update_LeaveCam(void)
{
    timer_leaveCam_.Update();
    const float rate = timer_leaveCam_.GetTimeRate(true);
    //const float pos_z = Math::Ease::EaseInCirc(rate, -50.f, -23.f);

    // Initialize()実行時の、プレイヤー追従カメラの座標を、スタート地点とする
    const Vector3 start = pos_leaveCamStart_;
    // 次の見下ろしカメラの座標を、エンド地点とする
    const Vector3 end = camera_wait_->GetTransform().position;

    // カメラの座標
    Vector3 pos{};
    // イージングする
    pos.x = Math::Ease::EaseInOutSine(rate, start.x, end.x);
    pos.y = Math::Ease::EaseInOutSine(rate, start.y, end.y);
    pos.z = Math::Ease::EaseInOutSine(rate, start.z, end.z);

    //Transform transform(Vector3{ 0.f,53.f, pos_z }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    Transform transform;
    transform.position = pos;
    camera_leave_->SetTransform(transform);
    camera_leave_->SetTargetPos(playerPtr_->GetTransform().position);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 次のタイマーを起動。
        timer_waitCam_.Start(kWaitTimer_);
        timer_waitCam_.SetAddSpeed(kCommonAddSpeed_);
        // 今のタイマーを停止
        timer_leaveCam_.Finish(true);
        // カメラの状態を変更
        cameraState_ = CameraState::WAIT;

        // 関数を抜ける
        return;
    }
}

void Event_TutorialPlanetHole::Update_WaitCam(void)
{
    timer_waitCam_.Update();
    const float rate = timer_waitCam_.GetTimeRate(true);

    Transform transform;
    transform.position = kCameraPos_wait;
    camera_wait_->SetTransform(transform);
    camera_wait_->SetTargetPos(playerPtr_->GetTransform().position);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 次のタイマーを起動。
        timer_approachCam_.Start(kApproachTimer_);
        timer_approachCam_.SetAddSpeed(kCommonAddSpeed_);
        // 今のタイマーを停止
        timer_waitCam_.Finish(true);
        // カメラの状態を変更
        cameraState_ = CameraState::APPROACH;

        // 関数を抜ける
        return;
    }
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
        // triggerがHole0用か
        const bool isHole0 = true;
        Initialize(isHole0);
    }
}

void Event_TutorialPlanetHole::OnTrigger_Hole1(void)
{
    // nullチェック
    if (!entrances_[1].GetOther()) { return; }

    if (entrances_[1].GetOther()->GetID() == "player")
    {
        // triggerがHole0用か
        const bool isHole0 = false;
        Initialize(isHole0);
    }
}
