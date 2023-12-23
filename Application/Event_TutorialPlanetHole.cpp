#include "Event_TutorialPlanetHole.h"
#include "CameraManager.h"
#include "BehindCamera.h"
#include "SimplifyImGui.h"
#include "MathUtil.h"

Event_TutorialPlanetHole::Event_TutorialPlanetHole(CollisionManager* arg_colMPtr, CameraManager* arg_cameraMPtr, Player* arg_playerPtr)
    : colMPtr_(arg_colMPtr), cameraMPtr_(arg_cameraMPtr), playerPtr_(arg_playerPtr)
{
    camera_interpolation1_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_interpolation_first");
    camera_leave_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_leave");
    camera_wait_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_wait");
    camera_approach_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_approach");
    camera_interpolation2_ = std::make_unique<NormalCamera>("event_tutorialPlanetHole_interpolation_last");
    cameraMPtr_->Register(camera_interpolation1_.get());
    cameraMPtr_->Register(camera_leave_.get());
    cameraMPtr_->Register(camera_wait_.get());
    cameraMPtr_->Register(camera_approach_.get());
    cameraMPtr_->Register(camera_interpolation2_.get());

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

#ifdef _DEBUG
    //is_showHoleCollision_ = true;
#endif // _DEBUG
}

Event_TutorialPlanetHole::~Event_TutorialPlanetHole(void)
{
    cameraMPtr_->UnRegister(camera_interpolation1_.get());
    cameraMPtr_->UnRegister(camera_leave_.get());
    cameraMPtr_->UnRegister(camera_wait_.get());
    cameraMPtr_->UnRegister(camera_approach_.get());
    cameraMPtr_->UnRegister(camera_interpolation2_.get());
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

    switch (cameraState_)
    {
    case Event_TutorialPlanetHole::CameraState::INTERPOLATION1:
        pos_player = points_playerSplineHole_.front();
        playerPtr_->SetPosition(pos_player);
        Update_InterpolationCam1();
        break;

    case Event_TutorialPlanetHole::CameraState::LEAVE:
        pos_player = Math::Function::Spline(points_playerSplineHole_, 1, timer_leaveCam_.GetTimeRate());
        playerPtr_->SetPosition(pos_player);
        //ImGui::Text("CameraState: Leave");
        Update_LeaveCam();
        break;

    case Event_TutorialPlanetHole::CameraState::WAIT:
        pos_player = Math::Function::Spline(points_playerSplineHole_, 2, timer_waitCam_.GetTimeRate());
        playerPtr_->SetPosition(pos_player);
        //ImGui::Text("CameraState: WAIT");
        Update_WaitCam();
        break;

    case Event_TutorialPlanetHole::CameraState::APPROACH:
        pos_player = Math::Function::Spline(points_playerSplineHole_, 3, timer_approachCam_.GetTimeRate());
        playerPtr_->SetPosition(pos_player);
        //ImGui::Text("CameraState: Approach");
        Update_ApproachCam();
        break;

    case Event_TutorialPlanetHole::CameraState::INTERPOLATION2:
        // 通過点の最後の座標にプレイヤーを移動し続けて、とどめる。
        // 今のやり方だと座標指定が無い場合、プレイヤーのUpdate()で重力がかかり続けるけど、押し戻しは停止し続けてるので、星の中央に囚われ続けてしまう。
        // 改善にはリファクタリングほぼ必須なので、最も安牌な選択肢である「ここでいるべき座標を指定し続ける」を実行。
        pos_player = points_playerSplineHole_.back();
        playerPtr_->SetPosition(pos_player);
        Update_InterpolationCam2();
        break;

    case Event_TutorialPlanetHole::CameraState::FINISH:
        // 起動フラグをfalse
        is_execute_ = false;
        // プレイヤーのeventStateを設定
        playerPtr_->SetEventState(PlayerEventState::NONE);
        break;

    default:
        break;
    }

#ifdef _DEBUG
    GUI::Begin("EventState_Hole");
    ImGui::Text("rate:%f", rate_player);
    ImGui::Text("current:%f", timer_player_.GetFrameCurrent());
    ImGui::Text("p.x:%f, p.y:%f, p.z:%f", pos_player.x, pos_player.y, pos_player.z);
    GUI::End();
#endif // _DEBUG

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
    // カメラの初期化
    const Axis3 axesInit = Axis3::Initialize();
    camera_interpolation2_->SetAxis3(axesInit); // 唯一forward以外の軸を変更しているため初期化。

    // タイマーの初期化
    timer_player_.Finish(true);
    timer_interpolation1_.Finish(true);
    timer_leaveCam_.Finish(true);
    timer_waitCam_.Finish(true);
    timer_approachCam_.Finish(true);
    timer_interpolation2_.Finish(true);
    // 各座標の初期化
    pos_leaveCamStart_ = {};
    pos_approachCamEnd_ = {};
    // 通過点初期化
    points_playerSplineHole_.clear();


    // イベントを起動
    is_execute_ = true;

    // イージング開始（プレイヤー用）
    const float totalTime = kLeaveTimer_ + kWaitTimer_ + kApproachTimer_;
    timer_player_.Start(totalTime);
    timer_player_.SetAddSpeed(kCommonAddSpeed_);
    // プレイヤーのeventStateを設定
    playerPtr_->SetEventState(PlayerEventState::PLANET_HOLE);

    // イージング開始（leaveCam用）
    timer_leaveCam_.Start(kLeaveTimer_);
    timer_leaveCam_.SetAddSpeed(kCommonAddSpeed_);
    // マネージャーにleaveCamをセット
    cameraMPtr_->SetCurrentCamera(camera_interpolation1_.get());

    // カメラの状態をLEAVEへ変更
    cameraState_ = CameraState::INTERPOLATION1;

    // カメラの最初の座標をプレイヤーのカメラの座標に合わせる。
    if (arg_isHole0)
    {
        // camera_leaveを指定の座標へセット（ Hole0用 )
        pos_leaveCamStart_ = planetPos_ + kCameraPos_leave_start;
        pos_approachCamEnd_ = planetPos_ + kCameraPos_approach_end;
        // camera_waitを指定の座標へセット（ Hole0用 ）
        const Vector3 pos_camWait = planetPos_ + kCameraPos_wait;
        camera_wait_->SetPosition(pos_camWait);

        // 穴に入って反対側まで行けるよう、スタート地点とゴール地点を設定
        const Vector3& pos_HoleStart = planetPos_ + kHolePos_relativePlanetCenter;
        const Vector3& pos_HoleEnd = planetPos_ - kHolePos_relativePlanetCenter;

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
        pos_approachCamEnd_ = planetPos_ - kCameraPos_approach_end;
        // camera_waitを指定の座標へセット（ Hole0用 ）
        const Vector3 pos_camWait = planetPos_ + kCameraPos_wait;
        camera_wait_->SetPosition(pos_camWait);

        // 穴に入って反対側まで行けるよう、スタート地点とゴール地点を設定
        const Vector3& pos_HoleStart = planetPos_ - kHolePos_relativePlanetCenter;
        const Vector3& pos_HoleEnd = planetPos_ + kHolePos_relativePlanetCenter;

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
        points_playerSplineHole_.push_back(kPlayerPos_Hole0_end);
        // プレイヤーが通る座標 (goal)
        points_playerSplineHole_.push_back(kPlayerPos_Hole0_end);
    }
}

void Event_TutorialPlanetHole::Update_InterpolationCam1(void)
{
    timer_interpolation1_.Update();
    const float rate = timer_interpolation1_.GetTimeRate(true);
    //const float pos_z = Math::Ease::EaseInCirc(rate, -50.f, -23.f);

    // Initialize()で定義した、pos_leaveCamStart_を、スタート地点とする
    const Vector3 start = points_playerSplineHole_.front();
    // camera_waitの座標を、エンド地点とする
    const Vector3 end = pos_leaveCamStart_;

    // カメラの座標
    Vector3 pos{};
    // イージングする
    pos.x = Math::Ease::EaseOutSine(rate, start.x, end.x);
    pos.y = Math::Ease::EaseOutSine(rate, start.y, end.y);
    pos.z = Math::Ease::EaseOutSine(rate, start.z, end.z);

    Transform transform;
    transform.position = pos;
    camera_interpolation1_->SetTransform(transform);
    camera_interpolation1_->SetTargetPos(playerPtr_->GetTransform().position);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 次のタイマーを起動。
        timer_leaveCam_.Start(kLeaveTimer_);
        timer_leaveCam_.SetAddSpeed(kCommonAddSpeed_);
        // 今のタイマーを停止
        timer_interpolation1_.Finish(true);

        // 次のカメラの初期情報をセット
        Transform transform_next;
        transform_next.position = kCameraPos_leave_start;
        camera_leave_->SetTransform(transform_next);
        camera_leave_->SetTargetPos(playerPtr_->GetTransform().position);
        // カメラの状態を変更
        cameraState_ = CameraState::LEAVE;
        // カメラを設定
        cameraMPtr_->SetCurrentCamera(camera_leave_.get());

        // 関数を抜ける
        return;
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
        // 次のタイマーを起動。
        timer_interpolation2_.Start(kApproachTimer_);
        timer_interpolation2_.SetAddSpeed(kCommonAddSpeed_);
        // 今のタイマーを停止
        timer_approachCam_.Finish(true);

        // きちんとイベント終了後、違和感なく遷移させるために必要な事。
        // 追従カメラがきちんとプレイヤーの真上にいさせる為に情報渡したり、追従カメラ自体にも更新をかけて情報通りの場所に移動させたり。
        playerPtr_->HandOverToBehindCamera("BehindCamera_follow_player0");
        cameraMPtr_->GetCamera("BehindCamera_follow_player0")->Update();

        // 次のカメラの初期情報をセット
        Transform transform_next;
        transform_next.position = pos_approachCamEnd_;
        camera_interpolation2_->SetTransform(transform_next);
        camera_interpolation2_->SetTargetPos(playerPtr_->GetTransform().position);
        // カメラの状態を変更
        cameraState_ = CameraState::INTERPOLATION2;
        // カメラを設定
        cameraMPtr_->SetCurrentCamera(camera_interpolation2_.get());

        // 関数を抜ける
        return;
    }
}

void Event_TutorialPlanetHole::Update_InterpolationCam2(void)
{
    timer_interpolation2_.Update();
    const float rate = timer_interpolation2_.GetTimeRate(true);

    // camera_approachの最終位置を、スタート地点とする
    const Vector3 start = pos_approachCamEnd_;
    // プレイヤーに追従するBehindCameraの位置を、エンド地点とする ※BehindCameraは、プレイヤーのUpdate()で渡された値を基に、座標を計算している。
    // Update_ApproachCam()の、タイマー完了時処理で呼び出しているプレイヤーの更新処理で、決定されたカメラの座標がエンド地点である。
    const Vector3 end = cameraMPtr_->GetCamera("BehindCamera_follow_player0")->GetTransform().position;


    // カメラの座標
    Vector3 pos{};
    // イージングする
    pos.x = Math::Ease::EaseOutSine(rate, start.x, end.x);
    pos.y = Math::Ease::EaseOutSine(rate, start.y, end.y);
    pos.z = Math::Ease::EaseOutSine(rate, start.z, end.z);
    // 構造体に定義
    Transform transform;
    transform.position = pos;
    // カメラに設定
    camera_interpolation2_->SetTransform(transform);

    // カメラの軸
    Axis3 axes;
    // 参照用の軸。最終的にこれと同じになる。
    const Axis3& behind_axes = cameraMPtr_->GetCamera("BehindCamera_follow_player0")->GetAxis3();
    // イージングなし。正面はプレイヤーの方向なので、そのまま使う。
    axes.forward = cameraMPtr_->GetCurrentCamera()->GetAxis3().forward;
    // イージング: right
    const Vector3 r = { 1.f,0.f,0.f }; // 仮想の右方向固定なので、それがスタート地点。
    axes.right.x = Math::Ease::EaseOutSine(rate, r.x, behind_axes.right.x);
    axes.right.y = Math::Ease::EaseOutSine(rate, r.y, behind_axes.right.y);
    axes.right.z = Math::Ease::EaseOutSine(rate, r.z, behind_axes.right.z);
    // イージング: up
    const Vector3 u = { 0.f,1.f,0.f }; // 仮想の上方向固定なので、それがスタート地点。
    axes.up.x = Math::Ease::EaseOutSine(rate, u.x, behind_axes.up.x);
    axes.up.y = Math::Ease::EaseOutSine(rate, u.y, behind_axes.up.y);
    axes.up.z = Math::Ease::EaseOutSine(rate, u.z, behind_axes.up.z);
    // カメラに設定
    camera_interpolation2_->SetAxis3(axes);


    //
    //camera_interpolation2_->SetTargetPos(playerPtr_->GetTransform().position);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 今のタイマーを停止
        timer_interpolation2_.Finish(true);

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
