#include "Event_Tutorial_StartCamera.h"
#include "SimplifyImGui.h"

void Event_Tutorial_StartCamera::SetIsExecute(bool arg_isExecute)
{
    is_execute_ = arg_isExecute;
    if (is_execute_ == false) { return; }
    Start();
    //playerPtr_->SetEventState(PlayerEventState::TUTORIAL_EVENT_ORDER);
}

void Event_Tutorial_StartCamera::Initialize(CameraManager* arg_cameraMPtr, Player* arg_playerPtr, Vector3* arg_cameraPosPtr, Axis3* arg_cameraPosturePtr_, bool* arg_isSnakeUpdate)
{
    cameraMPtr_ = arg_cameraMPtr;

    playerPtr_ = arg_playerPtr;
    cameraPosPtr_ = arg_cameraPosPtr;
    snakePosturePtr_ = arg_cameraPosturePtr_;
    isSnakeUpdate_ = arg_isSnakeUpdate;

    // 画像サイズ
    const Vector2 size{ 1280.f,80.f };
    // 画像の色
    const Vector4 color{ 0.f,0.f,0.f,1.f };
    // 画像の位置
    Vector2 position{ 0.f,0.f };

    // 上下の黒枠の生成と設定
    for (size_t i = 0; i < cinemas_.size(); i++)
    {
        cinemas_[i] = std::make_unique<Sprite>("Resources/white1x1.png");

        cinemas_[i]->SetSize(size);
        cinemas_[i]->SetColor(color);

        position = { 0.f, (720.f - size.y) * i }; // 2つ目は画面の下の位置へ
        cinemas_[i]->SetPosition(position);
    }

    blackScreen_ = std::make_unique<Sprite>("Resources/white1x1.png");
    blackScreen_->SetSize(Vector2{ WndAPI::kWidth_,WndAPI::kHeight_ });
    blackScreen_->SetColor(color);

    camera_ = std::make_unique<NormalCamera>("event_tutorial_startCamera");
    cameraMPtr_->Register(camera_.get());
    Transform transform(Vector3{ 0.f,53.f,-50.f }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    camera_->SetTransform(transform);
    camera_->SetPosition(kPos_camera1);
    camera_->SetAxis3(kPosture_egg);

    *isSnakeUpdate_ = false;
}

void Event_Tutorial_StartCamera::Execute(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

    for (size_t i = 0; i < 2; i++)
    {
        cinemas_[i]->Update();
    }
    blackScreen_->Update();

    // 秒数計測用のタイマーの更新
    timer_count_.Update();

    //## FadeIn処理 ##
    // fadeinのスピード用タイマーの更新
    timer_fadeinSpeed_.Update();
    // rateを取得
    float rate_fis = timer_fadeinSpeed_.GetTimeRate();

    // スピードを適用
    timer_fadein_.SetAddSpeed(rate_fis * kFadeInAddSpeedMax_);
    // fadein用タイマーの更新
    timer_fadein_.Update();
    // rateを取得
    float rate_fi = timer_fadein_.GetTimeRate();
    // 暗幕の透明度を計算して適用
    blackScreen_->SetAlpha(1 - rate_fi);

    // rateが、規定値を超えている
    if (rate_fi >= kTimeRate_playerTeleport_)
    {
        // プレイヤーの座標を移動
        playerPtr_->SetPosition(Vector3{ 2.5f, 34.5f, -38.5f });
    }

    //## FadeOut処理 ##
    // 秒数が規定値を超えているとき、fadeout用フラグをtrue
    if (timer_count_.GetFrameCurrent() >= kFinishEventEndTime_) { isOut_ = true; }
    // フラグが切り替わった瞬間
    if (isOutPre_ == false && isOut_)
    {
        // fadeout用のタイマーを開始
        timer_fadeout_.Start(kFadeOutTimer_);
        timer_fadeout_.SetAddSpeed(kFadeOutAddSpeed_);
    }
    // フラグ更新
    isOutPre_ = isOut_;
    // fadeout用のタイマーを更新
    timer_fadeout_.Update();
    float rate_fo = timer_fadeout_.GetTimeRate();
    // フラグがtrueの時のみ暗幕の透明度を計算して適用
    if (isOut_) { blackScreen_->SetAlpha(rate_fo); }

    //## チュートリアル開始のためのFadeIn処理 ##
    // 秒数が規定値を超えているとき、fadein用フラグをtrue
    if (rate_fo >= 1.f) { isIn_ = true; }
    if (isInPre_ == false && isIn_)
    {
        // fadein用のタイマーを開始
        timer_fadeinStartTutorial_.Start(kFadeInStartTutorialTimer_);
        timer_fadeinStartTutorial_.SetAddSpeed(kFadeInStartTutorialAddSpeed_);
        // カメラは一度プレイヤーカメラに再設定する。
        cameraMPtr_->SetCurrentCamera("BehindCamera_for_tutorial0");
        // fadein処理はこっちで継続して行う
    }
    // フラグ更新
    isInPre_ = isIn_;
    // fadein用のタイマーを更新
    timer_fadeinStartTutorial_.Update();
    float rate_fist = timer_fadeinStartTutorial_.GetTimeRate();
    if (isIn_) { blackScreen_->SetAlpha(1 - rate_fist); }
    if (rate_fist >= 1.f) { is_execute_ = false; }


    GUI::Begin("event");
    GUI::Text("timer_fadein: %f", timer_fadein_.GetFrameCurrent());
    GUI::Text("timer_count: %f", timer_count_.GetFrameCurrent());
    GUI::Text("timer_c1t2: %f", timer_camera1to2_.GetFrameCurrent());
    GUI::End();

    switch (cameraState_)
    {
    case Event_Tutorial_StartCamera::CameraState::CLOSE:
        Update_Close();
        break;
    case Event_Tutorial_StartCamera::CameraState::LOOK:
        Update_Look();
        break;
    case Event_Tutorial_StartCamera::CameraState::CHASE:
        Update_Chase();
        break;
    default:
        break;
    }

    //camera_->SetPosition(*cameraPosPtr_);

    //const Vector3 right = snakePosturePtr_->up.Cross(dir.Normalize());
    //camera_->SetAxis3({ dir.Normalize(),right,snakePosturePtr_->up });
    //camera_->SetAxis3(*snakePosturePtr_);
    //camera_->SetPosition(Vector3{ 0,60,-30 });
    //camera_->SetAxis3(Axis3{ {0.609f,0.220f,-0.76f},{-0.71f,0.574f,-0.403f},{0.348f,0.7885f,0.50694f} });
    //camera_->SetAxis3(Axis3{ {0.06f,0.03f,-0.706f},{-0.53f,0.684f,-0.497f},{0.4633f,0.72928f,0.50345f} });
    //camera_->SetAxis3(Axis3{ {0.1427f,0.4168f,-0.8977f},{-0.989f,0.285f,-0.1441f},{-0.035677f,0.90865f,0.416f} });
    //camera_->SetAxis3(Axis3{ {0,-1,0},{1,0,0},{0,0,1} });

    //switch (cameraState_)
    //{
    //case Event_Tutorial_StartCamera::CameraState::CLOSE:
    //    Update_CloseCam();
    //    break;

    //case Event_Tutorial_StartCamera::CameraState::WAIT1:
    //    Update_WaitCam();
    //    break;

    //case Event_Tutorial_StartCamera::CameraState::LEAVE:
    //    Update_LeaveCam();
    //    break;

    //case Event_Tutorial_StartCamera::CameraState::WAIT2:
    //    Update_WaitCam2();
    //    break;

    //case Event_Tutorial_StartCamera::CameraState::INTERPOLATE:
    //    Update_Interpolate();
    //    break;

    //case Event_Tutorial_StartCamera::CameraState::FINISH:
    //    is_execute_ = false;
    //    playerPtr_->SetEventState(PlayerEventState::NONE);
    //    break;

    //default:
    //    break;
    //}
}

void Event_Tutorial_StartCamera::Draw(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

    for (auto& sprite : cinemas_)
    {
        sprite->Draw();
    }
    blackScreen_->Draw();
}

void Event_Tutorial_StartCamera::Finalize(void)
{
    cameraMPtr_->UnRegister(camera_.get());
}

void Event_Tutorial_StartCamera::Start(void)
{
    cameraMPtr_->SetCurrentCamera(camera_.get());

    timer_count_.Start(kCountTimer_);
    timer_count_.SetAddSpeed(kCountAddSpeed_);
    timer_fadein_.Start(kFadeInTimer_);
    timer_fadein_.SetAddSpeed(kFadeInAddSpeed_);
    timer_fadeinSpeed_.Start(kFadeInSpeedTimer_);
    timer_fadeinSpeed_.SetAddSpeed(kFadeInSpeedAddSpeed_);
    timer_camera1to2_.Start(kCamera1to2Timer_);
    timer_camera1to2_.SetAddSpeed(kCamera1to2AddSpeed_);
}

void Event_Tutorial_StartCamera::Update_Close(void)
{
    timer_camera1to2_.Update();
    //const Vector3 dir = snakePosturePtr_->forward + snakePosturePtr_->right;
    //const Vector3 pos = *cameraPosPtr_ + -snakePosturePtr_->forward * 5;
    //const Vector3 pos = { -17.f,24.5f,61.f };
    float rate_c1t2 = timer_camera1to2_.GetTimeRate();
    Vector3 pos{};
    pos.x = Math::Ease::EaseInOutSine(rate_c1t2, kPos_camera1.x, kPos_camera2.x);
    pos.y = Math::Ease::EaseInOutSine(rate_c1t2, kPos_camera1.y, kPos_camera2.y);
    pos.z = Math::Ease::EaseInOutSine(rate_c1t2, kPos_camera1.z, kPos_camera2.z);
    camera_->SetPosition(pos);
    if (rate_c1t2 >= 1.f) 
    {
        *isSnakeUpdate_ = true;
        cameraState_ = CameraState::LOOK;
        timer_lookSnake_.Start(kLookSnakeTimer_);
        timer_lookSnake_.SetAddSpeed(kLookSnakeAddSpeed_);
    }
}

void Event_Tutorial_StartCamera::Update_Look(void)
{
    timer_lookSnake_.Update();
    float rate_ls = timer_lookSnake_.GetTimeRate();
    Axis3 posture{};
    posture.forward = Math::Ease3::EaseOutCubic(rate_ls, kPosture_egg.forward, kPosture_snake.forward);
    posture.right = Math::Ease3::EaseOutCubic(rate_ls, kPosture_egg.right, kPosture_snake.right);
    posture.up = Math::Ease3::EaseOutCubic(rate_ls, kPosture_egg.up, kPosture_snake.up);
    camera_->SetAxis3(posture);
    if (rate_ls >= 1.f)
    {
        cameraState_ = CameraState::CHASE;
        timer_chaseSnake_.Start(kChaseSnakeTimer_);
        timer_chaseSnake_.SetAddSpeed(kChaseSnakeAddSpeed_);
    }
}

void Event_Tutorial_StartCamera::Update_Chase(void)
{
    timer_chaseSnake_.Update();
    float rate_cs = timer_chaseSnake_.GetTimeRate();
    Axis3 posture{};
    posture.forward = Math::Ease3::EaseOutSine(rate_cs, kPosture_snake.forward, kPosture_snake_chase.forward);
    posture.right = Math::Ease3::EaseOutSine(rate_cs, kPosture_snake.right, kPosture_snake_chase.right);
    posture.up = Math::Ease3::EaseOutSine(rate_cs, kPosture_snake.up, kPosture_snake_chase.up);
    camera_->SetAxis3(posture);
}
