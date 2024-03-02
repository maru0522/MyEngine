#pragma once
#include "Sprite.h"
#include "NormalCamera.h"
#include "Timer.h"
#include "Player.h"

class Event_Tutorial_StartCamera
{
private:
    //>> 定義
    enum class CameraState
    {
        CLOSE,
        LOOK,
        CHASE,
    };

    const float kFinishEventEndTime_ = 10.5f;
    const Vector3 kPos_camera1 = { -20.6f,24.35f,64.f };
    const Vector3 kPos_camera2 = { -10.6f,24.5f,54.2f };
    const float kCamera1to2Timer_ = 9.f;        // CameraState::CLOSE
    const float kCamera1to2AddSpeed_ = 1.8f;
    const Axis3 kPosture_egg = Axis3{ Vector3{0.759565f,0.114823f,-0.640217f},Vector3{-0.51f,0.715f,-0.47f},Vector3{0.399585f,0.692843f,0.600251f} };
    const Axis3 kPosture_snake = Axis3{ Vector3{0.458572f,0.461871f,-0.759202f},Vector3{-0.8882f,0.299215f,-0.348669f},Vector3{0.057132f,0.836481f,0.545009f} };
    const Axis3 kPosture_snake_chase = Axis3{ Vector3{0.646436f,0.207559f,-0.734193f},Vector3{-0.685983f,0.585046f,-0.432607f},Vector3{0.332445f,0.788583f,0.517317f} };
    const float kLookSnakeTimer_ = 4.8f;         // CameraState::LOOK
    const float kLookSnakeAddSpeed_ = 1.9f;
    const float kChaseSnakeTimer_ = 6.f;        // CameraState::CHASE
    const float kChaseSnakeAddSpeed_ = 1.6f;

    const float kCountTimer_ = 1000.f;
    const float kCountAddSpeed_ = 1.f;
    const float kFadeInTimer_ = 14.f;
    const float kFadeInAddSpeed_ = 1.f;
    const float kFadeInAddSpeedMax_ = 7.f;
    const float kFadeInSpeedTimer_ = 12.f;
    const float kFadeInSpeedAddSpeed_ = 1.f;
    const float kFadeOutTimer_ = 6.f;
    const float kFadeOutAddSpeed_ = 2.2f;
    const float kFadeInStartTutorialTimer_ = 5.f;
    const float kFadeInStartTutorialAddSpeed_ = 1.8f;

    const float kTimeRate_playerTeleport_ = 0.75f; // fadeinTimerのrateがこの値の時、プレイヤーの座標を変更する
public:

    //>> 関数
    Event_Tutorial_StartCamera(void) = default;
    ~Event_Tutorial_StartCamera(void) = default;

    void Initialize(CameraManager* arg_cameraMPtr,Player* arg_playerPtr, Vector3* arg_cameraPosPtr, Axis3* arg_cameraPosturePtr_, bool* arg_isSnakeUpdate);
    void Execute(void);
    void Draw(void);
    void Finalize(void);

private:
    void Start(void);
    void Update_Close(void);
    void Update_Look(void);
    void Update_Chase(void);

    //>> 変数
    std::array<std::unique_ptr<Sprite>, 2> cinemas_;
    std::unique_ptr<Sprite> blackScreen_;

    CameraManager* cameraMPtr_;
    Player* playerPtr_;

    Vector3* cameraPosPtr_;
    Axis3* snakePosturePtr_;
    std::unique_ptr<NormalCamera> camera_;

    DeltaTimer timer_count_;
    DeltaTimer timer_fadein_;
    DeltaTimer timer_fadeinSpeed_;
    DeltaTimer timer_fadeout_;
    bool isOut_{}, isOutPre_{};
    DeltaTimer timer_fadeinStartTutorial_;
    bool isIn_{}, isInPre_{};

    CameraState cameraState_;
    DeltaTimer timer_camera1to2_;
    DeltaTimer timer_lookSnake_;
    DeltaTimer timer_chaseSnake_;
    

    bool* isSnakeUpdate_;
    bool is_execute_;

public:
    //>> setter
    void SetIsExecute(bool arg_isExecute);

    //>> getter
    bool GetIsExecite(void) { return is_execute_; }
};
