#include "Event_EndTutorial.h"
#include "SimplifyImGui.h"
#include "MathUtil.h"
#include "SceneManager.h"

Event_EndTutorial::Event_EndTutorial(void)
{
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

    const Vector2 sbgSize{ 530.f,70.f };
    const Vector4 sbgColor = { 0.f,0.f,0.f,0.f };
    // 文字用背景の生成と設定
    for (size_t i = 0; i < stringBackGrounds_.size(); i++)
    {
        stringBackGrounds_[i] = std::make_unique<Sprite>("Resources/white1x1.png");

        const Vector2 addSize = { 10.f * i,10.f * i };
        const Vector4 addColor = { 1.f * i, 1.f * i, 1.f * i, 0.f };
        stringBackGrounds_[i]->SetSize(sbgSize + addSize);
        stringBackGrounds_[i]->SetColor(sbgColor + addColor);
        stringBackGrounds_[i]->SetAnchorPoint(Vector2{ 0.5f,0.5f });
        stringBackGrounds_[i]->SetPosition(Vector2{ 640.f,360.f });
    }

    // 文字の生成と設定
    string_ = std::make_unique<Sprite>("Resources/string_clear!.png");
    string_->SetAnchorPoint(Vector2{ 0.5,0.5 });
    string_->SetPosition(Vector2{ 640,360 });
    string_->SetAlpha(0.f);

    camera_ = std::make_unique<NormalCamera>("event_endTutorial");
    CameraManager::GetInstance()->Register(camera_.get());
    Transform transform(Vector3{ 0.f,53.f,-50.f }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
    camera_->SetTransform(transform);

    cameraState_ = CameraState::CLOSE;
}

Event_EndTutorial::~Event_EndTutorial(void)
{
    CameraManager::GetInstance()->UnRegister(camera_.get());
}

void Event_EndTutorial::Initialize(void)
{
    CameraManager::GetInstance()->SetCurrentCamera(camera_.get());
    timer_closeCam_.Start(kCloseTimer_);
    timer_closeCam_.SetAddSpeed(kColseAddSpeed_);
}

void Event_EndTutorial::Execute(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

    for (size_t i = 0; i < 2; i++)
    {
        cinemas_[i]->Update();
        stringBackGrounds_[i]->Update();
    }
    string_->Update();

    //GUI::Begin("event_endTutorial");

    switch (cameraState_)
    {
    case Event_EndTutorial::CameraState::CLOSE:
        Update_CloseCam();
        break;

    case Event_EndTutorial::CameraState::WAIT1:
        Update_WaitCam();
        break;

    case Event_EndTutorial::CameraState::FINISH:
        is_execute_ = false;
        SceneManager::GetInstance()->RequestChangeScene(SceneName::TITLE);
        break;

    default:
        break;
    }

    //GUI::Text("camera : [%f,%f,%f]", camera_->GetTransform().position.x, camera_->GetTransform().position.y, camera_->GetTransform().position.z);
    //GUI::End();
}

void Event_EndTutorial::Draw(void)
{
    // 起動していないならスキップ
    if (is_execute_ == false) { return; }

    for (auto& sprite : cinemas_)
    {
        sprite->Draw();
    }

    if (cameraState_ == CameraState::WAIT1)
    {
        for (auto& sbg : stringBackGrounds_)
        {
            sbg->Draw();
        }
        string_->Draw();
    }
}

void Event_EndTutorial::Update_CloseCam(void)
{
    timer_closeCam_.Update();
    const float rate = timer_closeCam_.GetTimeRate(true);
    //const float pos_z = Math::Ease::EaseInCirc(rate, -50.f, -23.f);

    Vector3 pos(0.f, 53.f, -50.f);
    pos.z = Math::Ease::EaseInCirc(rate, -50.f, -23.f);

    //GUI::Text("closeCam : %f", rate);
    //GUI::Text("closeCam_posz : %f", pos.z);

    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // カメラ移動処理を一回だけ行う。
        //Transform transform(Vector3{ 0.f,53.f, pos_z }, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
        Transform transform(pos, Vector3{ 0.f,0.f,0.f }, Vector3{ 1.f,1.f,1.f });
        camera_->SetTransform(transform);

        // 次のタイマーを起動。
        timer_waitCam_.Start(kWaitTimer_);
        timer_waitCam_.SetAddSpeed(kWaitAddSpeed_);
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

void Event_EndTutorial::Update_WaitCam(void)
{
    timer_waitCam_.Update();
    const float rate = timer_waitCam_.GetTimeRate(true);
    //GUI::Text("waitCam : %f", rate);


    // 文字背景
    float rate_forSbg = std::clamp(rate * 2.f, 0.f, 0.4f);
    if (rate >= 0.8f)
    {
        rate_forSbg = 2.f - rate * 2;
    }
    for (auto& sbg : stringBackGrounds_)
    {
        sbg->SetAlpha(rate_forSbg);
    }
    // 文字
    float rate_forStr = std::clamp(rate_forSbg / 0.4f, 0.f, 1.f);
    string_->SetAlpha(rate_forStr);


    // タイマーが完了しているか
    if (rate >= 1.f)
    {
        // 今のタイマーを停止
        timer_waitCam_.Finish(true);

        cameraState_ = CameraState::FINISH;
        CameraManager::GetInstance()->SetCurrentCamera("SphericalCamera_follow_player0");

        // 関数を抜ける
        return;
    }
}

void Event_EndTutorial::SetIsExecute(bool arg_isExecute)
{
    is_execute_ = arg_isExecute;
    if (is_execute_) { Initialize(); }
}
