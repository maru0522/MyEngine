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
    cameraMPtr_->Register(camera_leave_.get());
    cameraMPtr_->Register(camera_wait_.get());
    cameraMPtr_->Register(camera_approach_.get());

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
    cameraMPtr_->UnRegister(camera_leave_.get());
    cameraMPtr_->UnRegister(camera_wait_.get());
    cameraMPtr_->UnRegister(camera_approach_.get());
    colMPtr_->UnRegister(&entrances_[0]);
    colMPtr_->UnRegister(&entrances_[1]);
}

void Event_TutorialPlanetHole::Execute(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

    // プレイヤーの座標を、穴を通り抜けるように更新し続ける。
    timer_player_.Update();
    const float rate_player = timer_player_.GetTimeRate();

    Vector3 pos_player{};
    if (timer_player_.GetIsExecute() == false)
    {
        playerPtr_->SetPosition(kPlayerPos_Hole1_end);
    }
    GUI::Begin("EventState_Hole");

    switch (cameraState_)
    {
    case Event_TutorialPlanetHole::CameraState::LEAVE:
        pos_player = Math::Function::Spline(points_playerSplineHole_, 1, timer_leaveCam_.GetTimeRate());
        ImGui::Text("CameraState: Leave");
        Update_LeaveCam();
        break;

    case Event_TutorialPlanetHole::CameraState::WAIT:
        pos_player = Math::Function::Spline(points_playerSplineHole_, 2, timer_waitCam_.GetTimeRate());
        ImGui::Text("CameraState: WAIT");
        Update_WaitCam();
        break;

    case Event_TutorialPlanetHole::CameraState::APPROACH:
        pos_player = Math::Function::Spline(points_playerSplineHole_, 3, timer_approachCam_.GetTimeRate());
        ImGui::Text("CameraState: Approach");
        Update_ApproachCam();
        break;

    case Event_TutorialPlanetHole::CameraState::FINISH:
        // 起動フラグをfalse
        is_execute_ = false;
        // プレイヤーのeventStateを設定
        playerPtr_->SetEventState(Player::EventState::NONE);
        break;

    default:
        break;
    }

    playerPtr_->SetPosition(pos_player);

    ImGui::Text("rate:%f", rate_player);
    ImGui::Text("current:%f", timer_player_.GetFrameCurrent());
    ImGui::Text("p.x:%f, p.y:%f, p.z:%f", pos_player.x, pos_player.y, pos_player.z);
    GUI::End();

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
    // タイマーの初期化
    timer_leaveCam_.Finish(true);
    timer_waitCam_.Finish(true);
    timer_approachCam_.Finish(true);
    timer_player_.Finish(true);
    // 各座標の初期化
    pos_leaveCamStart_ = {};
    pos_approachCamEnd_ = {};
    pos_playerStart_ = {};
    pos_playerEnd_ = {};

    // イベントを起動
    is_execute_ = true;

    // イージング開始（プレイヤー用）
    const float totalTime = kLeaveTimer_ + kWaitTimer_ + kApproachTimer_;
    timer_player_.Start(totalTime);
    timer_player_.SetAddSpeed(kCommonAddSpeed_);
    // プレイヤーのeventStateを設定
    playerPtr_->SetEventState(Player::EventState::PLANET_HOLE);

    // イージング開始（leaveCam用）
    timer_leaveCam_.Start(kLeaveTimer_);
    timer_leaveCam_.SetAddSpeed(kCommonAddSpeed_);
    // マネージャーにleaveCamをセット
    cameraMPtr_->SetCurrentCamera(camera_leave_.get());

    // カメラの状態をLEAVEへ変更
    cameraState_ = CameraState::LEAVE;

    // カメラの最初の座標をプレイヤーのカメラの座標に合わせる。
    if (arg_isHole0)
    {
        // camera_leaveを指定の座標へセット（ Hole0用 )
        pos_leaveCamStart_ = planetPos_ + kCameraPos_leave_start;
        pos_approachCamEnd_ = planetPos_ - kCameraPos_leave_start;
        // camera_waitを指定の座標へセット（ Hole0用 ）
        const Vector3 pos_camWait = planetPos_ + kCameraPos_wait;
        camera_wait_->SetPosition(pos_camWait);

        // 穴に入って反対側まで行けるよう、スタート地点とゴール地点を設定
        const Vector3& pos_HoleStart = planetPos_ + kHolePos_relativePlanetCenter;
        const Vector3& pos_HoleEnd = planetPos_ - kHolePos_relativePlanetCenter;

        pos_playerStart_ = planetPos_ + kHolePos_relativePlanetCenter;
        pos_playerEnd_ = planetPos_ - kHolePos_relativePlanetCenter;

        // イベントトリガーに触れた瞬間のプレイヤーの座標
        const Vector3& pos_contactTrigger = playerPtr_->GetTransform().position;
        // プレイヤーが通る座標 (start)
        points_playerSplineHole_.push_back(pos_contactTrigger); // 関数の仕様上、スタート地点とゴール地点は2回入力する必要がある。 // TODO: スプライン曲線の関数見直し。
        // プレイヤーが通る座標 1番目
        points_playerSplineHole_.push_back(pos_contactTrigger);
        // プレイヤーが通る座標 2番目
        points_playerSplineHole_.push_back(pos_HoleStart);
        // プレイヤーが通る座標 3番目
        points_playerSplineHole_.push_back(pos_HoleEnd);
        // プレイヤーが通る座標 4番目
        points_playerSplineHole_.push_back(kPlayerPos_Hole1_end);
        // プレイヤーが通る座標 (goal)
        points_playerSplineHole_.push_back(kPlayerPos_Hole1_end);
    }
    else
    {
        // camera_leaveを指定の座標へセット（ Hole0用 )
        pos_leaveCamStart_ = planetPos_ - kCameraPos_leave_start;
        pos_approachCamEnd_ = planetPos_ + kCameraPos_leave_start;
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

    // Initialize()で定義した、pos_leaveCamStart_を、スタート地点とする
    const Vector3 start = pos_leaveCamStart_;
    // camera_waitの座標を、エンド地点とする
    const Vector3 end = camera_wait_->GetTransform().position;

    // カメラの座標
    Vector3 pos{};
    // イージングする
    pos.x = Math::Ease::EaseOutSine(rate, start.x, end.x);
    pos.y = Math::Ease::EaseOutSine(rate, start.y, end.y);
    pos.z = Math::Ease::EaseOutSine(rate, start.z, end.z);

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

        // 次のカメラの初期情報をセット
        Transform transform_next;
        transform_next.position = kCameraPos_wait;
        camera_wait_->SetTransform(transform_next);
        camera_wait_->SetTargetPos(playerPtr_->GetTransform().position);
        // カメラの状態を変更
        cameraState_ = CameraState::WAIT;
        // カメラを設定
        cameraMPtr_->SetCurrentCamera(camera_wait_.get());

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

        // 次のカメラの初期情報をセット
        Transform transform_next;
        transform_next.position = kCameraPos_wait;
        camera_approach_->SetTransform(transform_next);
        camera_approach_->SetTargetPos(playerPtr_->GetTransform().position);
        // カメラの状態を変更
        cameraState_ = CameraState::APPROACH;
        // カメラを設定
        cameraMPtr_->SetCurrentCamera(camera_approach_.get());

        // 関数を抜ける
        return;
    }
}

void Event_TutorialPlanetHole::Update_ApproachCam(void)
{
    timer_approachCam_.Update();
    const float rate = timer_approachCam_.GetTimeRate(true);

    // camera_waitの座標を、スタート地点とする
    const Vector3 start = camera_wait_->GetTransform().position;
    // Initialize()で定義した、pos_approachCamEnd_を、エンド地点とする
    const Vector3 end = pos_approachCamEnd_;

    // カメラの座標
    Vector3 pos{};
    // イージングする
    pos.x = Math::Ease::EaseOutSine(rate, start.x, end.x);
    pos.y = Math::Ease::EaseOutSine(rate, start.y, end.y);
    pos.z = Math::Ease::EaseOutSine(rate, start.z, end.z);

    Transform transform;
    transform.position = pos;
    camera_approach_->SetTransform(transform);
    camera_approach_->SetTargetPos(playerPtr_->GetTransform().position);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 今のタイマーを停止
        timer_waitCam_.Finish(true);

        // プレイヤーの更新処理を1回呼び、追従カメラの座標を更新する。
        playerPtr_->Update();

        // カメラの状態を変更
        cameraState_ = CameraState::FINISH;
        //cameraMPtr_->SetCurrentCamera("SphericalCamera_follow_player0");
        cameraMPtr_->SetCurrentCamera("BehindCamera_follow_player0");

        // 関数を抜ける
        return;
    }
}

void Event_TutorialPlanetHole::OnTrigger_Hole0(void)
{
    // nullチェック
    if (!entrances_[0].GetOther()) { return; }
    // 既に起動済みならスキップ
    if (is_execute_) { return; }

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
    // 既に起動済みならスキップ
    if (is_execute_) { return; }

    if (entrances_[1].GetOther()->GetID() == "player")
    {
        // triggerがHole0用か
        const bool isHole0 = false;
        Initialize(isHole0);
    }
}
