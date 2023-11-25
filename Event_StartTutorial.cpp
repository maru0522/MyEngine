#include "Event_StartTutorial.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

Event_StartTutorial::Event_StartTutorial(void)
{
    // 画像サイズ
    const Vector2 size{ 1280.f,60.f };
    // 画像の色
    const Vector4 color{ 0.f,0.f,0.f,1.f };
    // 画像の位置
    Vector2 position{ 0.f,0.f };

    for (size_t i = 0; i < cinemas_.size(); i++)
    {
        cinemas_[i] = std::make_unique<Sprite>("Resources/white1x1.png");

        cinemas_[i]->SetSize(size);
        cinemas_[i]->SetColor(color);

        position = { 0.f, 660.f * i }; // 2つ目は画面の下の位置へ
        cinemas_[i]->SetPosition(position);
    }

    camera_ = std::make_unique<NormalCamera>("event_tutorial");
    CameraManager::GetInstance()->Register(camera_.get());
    CameraManager::GetInstance()->SetCurrentCamera(camera_.get());
    Transform transform(Vector3{ 0.f,53.f,-50.f }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    camera_->SetTransform(transform);

    timer_closeCam_.Start(6.f);
    timer_closeCam_.SetAddSpeed(2.f);

    cameraState_ = CameraState::CLOSE;
}

Event_StartTutorial::~Event_StartTutorial(void)
{
    CameraManager::GetInstance()->UnRegister(camera_.get());
}

void Event_StartTutorial::Execute(void)
{
    for (auto& sprite : cinemas_)
    {
        sprite->Update();
    }

    GUI::Begin("event_startTutorial");

    switch (cameraState_)
    {
    case Event_StartTutorial::CameraState::CLOSE:
        Update_CloseCam();
        break;

    case Event_StartTutorial::CameraState::WAIT1:
        Update_WaitCam();
        break;

    case Event_StartTutorial::CameraState::LEAVE:
        Update_LeaveCam();
        break;

    case Event_StartTutorial::CameraState::WAIT2:
        break;

    default:
        break;
    }

    GUI::Text("camera : [%f,%f,%f]", camera_->GetTransform().position.x, camera_->GetTransform().position.y, camera_->GetTransform().position.z);
    GUI::End();
}

void Event_StartTutorial::Draw(void)
{
    for (auto& sprite : cinemas_)
    {
        sprite->Draw();
    }
}

void Event_StartTutorial::Update_CloseCam(void)
{
    timer_closeCam_.Update();
    const float rate = timer_closeCam_.GetTimeRate(true);
    //const float pos_z = Math::Ease::EaseInCirc(rate, -50.f, -23.f);

    Vector3 pos(0.f, 53.f, -50.f);
    pos.z = Math::Ease::EaseInCirc(rate, -50.f, -23.f);

    GUI::Text("closeCam : %f", rate);
    GUI::Text("closeCam_posz : %f", pos.z);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // カメラ移動処理を一回だけ行う。
        //Transform transform(Vector3{ 0.f,53.f, pos_z }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
        Transform transform(pos, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
        camera_->SetTransform(transform);

        // 次のタイマーを起動。
        timer_waitCam_.Start(5.f);
        // 今のタイマーを停止
        timer_closeCam_.Finish(true);

        cameraState_ = CameraState::WAIT1;

        // 関数を抜ける
        return;
    }

    //Transform transform(Vector3{ 0.f,53.f, pos_z }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    Transform transform;
    transform.position = pos;
    camera_->SetTransform(transform);
}

void Event_StartTutorial::Update_WaitCam(void)
{
    timer_waitCam_.Update();
    const float rate = timer_waitCam_.GetTimeRate(true);
    GUI::Text("waitCam : %f", rate);


    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 次のタイマーを起動。
        timer_leaveCam_.Start(8.f);
        timer_leaveCam_.SetAddSpeed(1.2f);
        // 今のタイマーを停止
        timer_waitCam_.Finish(true);

        cameraState_ = CameraState::LEAVE;

        // 関数を抜ける
        return;
    }
}

void Event_StartTutorial::Update_LeaveCam(void)
{
    timer_leaveCam_.Update();
    const float rate = timer_leaveCam_.GetTimeRate();
    GUI::Text("pukkCam : %f", rate);
    if (rate > 0.2f) { timer_leaveCam_.SetAddSpeed(rate + 1.2f); }

    Vector3 pos;
    pos.x = Math::Ease::EaseInCubic(rate, 0.f, 86.f);
    pos.y = Math::Ease::EaseInCubic(rate, 53.f, 54.f);
    pos.z = Math::Ease::EaseInCubic(rate, -23.f, -382.f);

    Transform transform(pos, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    camera_->SetTransform(transform);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 今のタイマーを停止
        timer_leaveCam_.Finish(true);

        cameraState_ = CameraState::WAIT2;

        // 関数を抜ける
        return;
    }
}
