#include "PlayerBehavior.h"
#include "Input.h"
#include "Vector3.h"
#include "Player.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"
#include "PostEffectManager.h"
#include "RadialBlur.h"
#include "CameraManager.h"
#include "SphericalCamera.h"
#include "WndAPI.h"
#include "Screen.h"
#include "CollisionChecker.h"
#include "UI.h"
#include "BehindCamera.h"

//----------------------------------------------------------------------------------------
std::unique_ptr<IPlayerBehavior> PlayerBehaviorFactory::Create(Player* arg_playerPtr, PlayerBehavior arg_state)
{
    if (arg_state == PlayerBehavior::IDLE) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::STOOP) { return std::make_unique<PlayerBehavior_Stoop>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::MOVE) { return std::make_unique<PlayerBehavior_Move>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::MOVE_STOOP) { return std::make_unique<PlayerBehavior_MoveStoop>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP) { return std::make_unique<PlayerBehavior_Jump>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP_STOOP) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP_REVERSE) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP_LONG) { return std::make_unique<PlayerBehavior_JumpLong>(arg_playerPtr); }

    return std::unique_ptr<IPlayerBehavior>();
}

//----------------------------------------------------------------------------------------
void PlayerBehaviorMachine::Initialize(Player* arg_playerPtr, PlayerBehavior arg_state)
{
    playerPtr_ = arg_playerPtr;
    statePtr_ = stateFactory_.Create(arg_playerPtr, arg_state);
}

void PlayerBehaviorMachine::ManagementBehavior(void)
{
    // nullチェック
    if (!statePtr_) { return; }

    // 状態遷移条件を満たしているか確認し、遷移する
    NextStateCheck();

    // 状態更新
    statePtr_->Execute();
}

void PlayerBehaviorMachine::NextStateCheck(void)
{
    // 状態遷移するかどうか確認する
    statePtr_->RequirementCheck(); // 状態遷移する場合は nextStateを変更する

    // nextStateが"NONE"以外である場合、状態遷移を行う
    PlayerBehavior nextState = statePtr_->GetNextState();
    if (nextState != PlayerBehavior::NONE)
    {
        // 既存のstateがあれば終了処理
        statePtr_->Exit();
        // 上書き
        statePtr_.reset();
        statePtr_ = stateFactory_.Create(playerPtr_, nextState);
        // 初期化処理
        statePtr_->Entry();

        // それぞれの、"振舞い"に必要な情報をあてがう
    }
}

//----------------------------------------------------------------------------------------
//IDLE,          //
//STOOP,         // しゃがみ
//
//MOVE,          // 移動
//MOVE_STOOP,    // しゃがみ移動
//
//JUMP,          // ジャンプ
//JUMP_STOOP,    // じゃがみジャンプ
//JUMP_REVERSE,  // 反転ジャンプ
//JUMP_LONG,     // 幅跳び

void PlayerBehavior_Idle::Execute(void) // "IDLE"
{
    // 1フレーム前の3軸を記録
    commonInfo_->axes_old_ = commonInfo_->axes_;

    // ステート確認用
    debug_curState_ = PlayerBehavior::IDLE;

    // 重力
    Process_Gravity();

    // 移動量 = 上方向 * ジャンプ量
    const Vector3 velocity = commonInfo_->axes_.up * commonInfo_->jumpVecNorm_; // Idle状態は重力以外の移動量は発生しない想定
    Process_Transform(velocity);

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right.Normalize(), commonInfo_->axes_.up.Normalize()); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up.Normalize(), pForwardFromCamera.Normalize()); // 右Vec: cross(p.upVec, pForwardFromCamera)
    // 定義した値等を現在の姿勢として記録
    commonInfo_->axes_.forward = pForwardFromCamera.Normalize();
    commonInfo_->axes_.right = redefinitionPRightFromCamera.Normalize();
    commonInfo_->axes_.up = commonInfo_->axes_.up.Normalize();

    // カメラのptrをカメラマネージャーから取得
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    // カメラIDの接頭辞が、"BehindCamera_"以外なら、スキップ
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }

    // カメラのptrを基底クラスのptr[ICamera]から、[BehindCamera]にキャスト
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    // カメラの姿勢を、プレイヤーの姿勢を利用して設定。
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    // カメラの中心点を、プレイヤーの座標として入力。
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;
}

void PlayerBehavior_Idle::RequirementCheck(void)
{
    //イベント中の場合、判定を行わない。
    if (commonInfo_->eventState_ != PlayerEventState::NONE) { return; }

    const bool isDown_LT = XPAD::GetLT();
    const bool isDown_LStick = (bool)(XPAD::GetLStick().Length());
    const bool isDown_AorB = XPAD::IsDown(XPAD::Button::A) || XPAD::IsDown(XPAD::Button::B);

    const bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    const bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    const bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);

    const bool isStoop = isDown_LT + isDown_LSHIFT;
    const bool isMove = isDown_LStick + isDown_anyWASD;
    const bool isJump = isDown_AorB + isDown_SPACE;
    bool isLanding = commonInfo_->isLanding_;

    // 左シフトが入力されている
    if (isStoop && commonInfo_->is_carrySnake_ == false)
    {
        // PlayerState を STOOP(しゃがみ)へ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }

    // 移動キーが入力されている
    if (isMove)
    {
        // PlayerState を MOVE(移動)へ
        nextState_ = PlayerBehavior::MOVE;
        return;
    }

    // SPACEが入力されている && 地面に足がついている
    if (isJump && isLanding)
    {
        // PlayerState を JUMP(ジャンプ)へ
        nextState_ = PlayerBehavior::JUMP;
        return;
    }
}

void PlayerBehavior_Stoop::Entry(void)
{
    // stoop状態時用のカメラ距離を適応するフラグをtrueに
    commonInfo_->is_close_ = true;
    // カメラを近づける用のイージングタイマーを初期化 && 起動
    commonInfo_->timer_stoop_cameraClose_.Finish(true);
    commonInfo_->timer_stoop_cameraClose_.Start(commonInfo_->kTimer_stoop_cameraClose_);
}

void PlayerBehavior_Stoop::Execute(void) // "STOOP"
{
    // 1フレーム前の3軸を記録
    commonInfo_->axes_old_ = commonInfo_->axes_;

    // ステート確認用
    debug_curState_ = PlayerBehavior::STOOP;

    // 重力
    Process_Gravity();

    // 移動量 = 上方向 * ジャンプ量
    const Vector3 velocity = commonInfo_->axes_.up * commonInfo_->jumpVecNorm_; // stoop状態は重力以外の移動量は発生しない想定（ただその場でしゃがんでるだけだから）
    Process_Transform(velocity);

    // カメラのptrをカメラマネージャーから取得
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    // カメラIDの接頭辞が、"BehindCamera_"以外なら、スキップ
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }

    // カメラのptrを基底クラスのptr[ICamera]から、[BehindCamera]にキャスト
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    // カメラの姿勢を、プレイヤーの姿勢を利用して設定。
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    // カメラの中心点を、プレイヤーの座標として入力。
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Stoop::Exit(void)
{
    // stoop状態時用のカメラ距離を適応するフラグをfalseに
    commonInfo_->is_close_ = false;
    // カメラが離れる用のイージングタイマーを初期化 && 起動
    commonInfo_->timer_stoop_cameraLeave_.Finish(true);
    commonInfo_->timer_stoop_cameraLeave_.Start(commonInfo_->kTimer_stoop_cameraLeave_);
}

void PlayerBehavior_Stoop::RequirementCheck(void)
{
    //イベント中の場合、判定を行わない。
    if (commonInfo_->eventState_ != PlayerEventState::NONE) { return; }

    const bool isDown_LT = XPAD::GetLT();
    const bool isDown_LStick = (bool)(XPAD::GetLStick().Length());
    const bool isDown_AorB = XPAD::IsDown(XPAD::Button::A) || XPAD::IsDown(XPAD::Button::B);

    const bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    const bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    const bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);

    const bool isStoop = isDown_LT + isDown_LSHIFT;
    const bool isMove = isDown_LStick + isDown_anyWASD;
    const bool isJump = isDown_AorB + isDown_SPACE;
    bool isLanding = commonInfo_->isLanding_;

    // 左SHIFTが入力されていない && 移動キーが入力されていない && SPACEが入力されている
    if (isStoop == false && isMove == false && isJump == false)
    {
        // PlayerState を Idleへ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 左SHIFTが入力されている && 移動キーが入力されている
    if (isMove) // ここを通っている時点で、実質左SHIFTが入力されている
    {
        // 蛇を運んでいる場合は、STOOPにはならない
        if (commonInfo_->is_carrySnake_)
        {
            // PlayerState を MOVEへ
            nextState_ = PlayerBehavior::MOVE;
            return;
        }
        else
        {
            // PlayerState を MOVE_STOOP(しゃがみ移動)へ
            nextState_ = PlayerBehavior::MOVE_STOOP;
            return;
        }
    }

    // SPACEが入力されている && 地面に足がついている
    if (isJump && isLanding)
    {
        // PlayerState を JUMP_STOOP(しゃがみジャンプ)へ
        nextState_ = PlayerBehavior::JUMP_STOOP;
        return;
    }
}

void PlayerBehavior_Move::Execute(void) // "MOVE"
{
    // 1フレーム前の3軸を記録
    commonInfo_->axes_old_ = commonInfo_->axes_;

    // ステート確認用
    debug_curState_ = PlayerBehavior::MOVE;

    // 入力ベクトルの取得。
    Vector2 vec2_input = Process_GetInput();


    // モデル用のaxes計算
    Process_CalculateModelAxes(vec2_input);

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right.Normalize(), commonInfo_->axes_.up.Normalize()); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up.Normalize(), pForwardFromCamera.Normalize()); // 右Vec: cross(p.upVec, pForwardFromCamera)
    // 定義した値等を現在の姿勢として記録
    commonInfo_->axes_.forward = pForwardFromCamera.Normalize();
    commonInfo_->axes_.right = redefinitionPRightFromCamera.Normalize();
    commonInfo_->axes_.up = commonInfo_->axes_.up.Normalize();


    // 重力 ※ジャンプ量に加算してる
    Process_Gravity();
    // 移動vec = (前後vec * 入力vec.y) + (水平vec * 入力vec.y)
    Vector3 moveVec = (commonInfo_->axes_.forward * vec2_input.y) + (commonInfo_->axes_.right * vec2_input.x);

    float spd = commonInfo_->kMoveSpeed_;
    if (commonInfo_->is_carrySnake_) { spd = commonInfo_->kMoveSpeed_ / 3.f; }
    // 移動量 = 移動vec * 移動速度 + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * (spd + commonInfo_->coinNum_ * 0.01f * commonInfo_->kMoveSpeed_)) + (commonInfo_->axes_.up * commonInfo_->jumpVecNorm_);
    // 座標更新
    Process_Transform(velocity);


    // カメラ座標用の値を補正
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;


    // 入力ベクトルが0でないなら、1フレーム前の入力ベクトルを記録
    if (vec2_input.IsNonZero()) { commonInfo_->vec2_input_old_ = vec2_input; }
#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Move::RequirementCheck(void)
{
    //イベント中の場合、判定を行わない。
    if (commonInfo_->eventState_ != PlayerEventState::NONE) { return; }

    const bool isDown_LT = XPAD::GetLT();
    const bool isDown_LStick = (bool)(XPAD::GetLStick().Length());
    const bool isDown_AorB = XPAD::IsDown(XPAD::Button::A) || XPAD::IsDown(XPAD::Button::B);

    const bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    const bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    const bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);

    const bool isStoop = isDown_LT + isDown_LSHIFT;
    const bool isMove = isDown_LStick + isDown_anyWASD;
    const bool isJump = isDown_AorB + isDown_SPACE;
    bool isLanding = commonInfo_->isLanding_;

    // しゃがみ入力がされていない && 移動入力をしていない && ジャンプ入力をしていない
    if (isStoop == false && isMove == false && isJump == false)
    {
        // PlayerState をIDLEに変更する
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // しゃがみ入力がされている && 着地している && 蛇を運んでいない。
    if (isStoop && isLanding && commonInfo_->is_carrySnake_ == false)
    {
        // PlayerState をMOVE_STOOPに変更する
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
    }

    // ジャンプ入力をしている && 着地している
    if (isJump && isLanding)
    {
        // PlayerState をJUMPに変更する
        nextState_ = PlayerBehavior::JUMP;
        return;
    }
}

void PlayerBehavior_MoveStoop::Entry(void)
{
    // stoop状態時用のカメラ距離を適応するフラグをtrueに
    commonInfo_->is_close_ = true;
    // カメラを近づける用のイージングタイマーを初期化 && 起動
    commonInfo_->timer_stoop_cameraClose_.Finish(true);
    commonInfo_->timer_stoop_cameraClose_.Start(commonInfo_->kTimer_stoop_cameraClose_);
}

void PlayerBehavior_MoveStoop::Execute(void)
{
    // 1フレーム前の3軸を記録
    commonInfo_->axes_old_ = commonInfo_->axes_;

    // ステート確認用
    debug_curState_ = PlayerBehavior::MOVE_STOOP;

    // 入力ベクトルの取得。
    Vector2 vec2_input = Process_GetInput();

    // モデル用のaxes計算
    Process_CalculateModelAxes(vec2_input);

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right, commonInfo_->axes_.up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)
    // 定義した値等を現在の姿勢として記録
    commonInfo_->axes_.forward = pForwardFromCamera;
    commonInfo_->axes_.right = redefinitionPRightFromCamera;
    commonInfo_->axes_.up = commonInfo_->axes_.up;

    // 重力 ※ジャンプ量に加算してる
    Process_Gravity();
    // 移動vec = (前後vec * 入力vec.y) + (水平vec * 入力vec.y)
    Vector3 moveVec = (commonInfo_->axes_.forward * vec2_input.y) + (commonInfo_->axes_.right * vec2_input.x);
    // 移動速度除算値
    const float divedeSpeed = 3.f;
    // 移動量 = 移動vec * (移動速度 / 規定割合) + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * (commonInfo_->kMoveSpeed_ / divedeSpeed)) + (commonInfo_->axes_.up * commonInfo_->jumpVecNorm_);
    // 座標更新
    Process_Transform(velocity);

    // カメラ座標用の値を補正
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;


    // 入力ベクトルが0でないなら、1フレーム前の入力ベクトルを記録
    if (vec2_input.IsNonZero()) { commonInfo_->vec2_input_old_ = vec2_input; }
}

void PlayerBehavior_MoveStoop::Exit(void)
{
    // stoop状態時用のカメラ距離を適応するフラグをfalseに
    commonInfo_->is_close_ = false;
    // カメラが離れる用のイージングタイマーを初期化 && 起動
    commonInfo_->timer_stoop_cameraLeave_.Finish(true);
    commonInfo_->timer_stoop_cameraLeave_.Start(commonInfo_->kTimer_stoop_cameraLeave_);
}

void PlayerBehavior_MoveStoop::RequirementCheck(void)
{
    //イベント中の場合、判定を行わない。
    if (commonInfo_->eventState_ != PlayerEventState::NONE) { return; }

    const bool isDown_LT = XPAD::GetLT();
    const bool isDown_LStick = (bool)(XPAD::GetLStick().Length());
    const bool isDown_AorB = XPAD::IsDown(XPAD::Button::A) || XPAD::IsDown(XPAD::Button::B);
    const bool isTrigger_AorB = XPAD::IsTrigger(XPAD::Button::A) || XPAD::IsTrigger(XPAD::Button::B);

    const bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    const bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    const bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    const bool isTrigger_SPACE = KEYS::IsTrigger(DIK_SPACE);

    const bool isStoop = isDown_LT + isDown_LSHIFT;
    const bool isMove = isDown_LStick + isDown_anyWASD;
    const bool isJump = isDown_AorB + isDown_SPACE;
    const bool isJumpTrigger = isTrigger_AorB + isTrigger_SPACE;
    bool isLanding = commonInfo_->isLanding_;

    // 左SHIFTが入力されていない && 移動キーが入力されていない && SPACEが入力されている
    if (isStoop == false && isMove == false && isJump == false)
    {
        // PlayerState を Idleへ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 左SHIFTが入力されていない
    if (isStoop == false)
    {
        // PlayerState を MOVE(移動)へ
        nextState_ = PlayerBehavior::MOVE;
        return;
    }
    // 蛇を運んでいる
    if (commonInfo_->is_carrySnake_)
    {
        // PlayerState を MOVE(移動)へ
        nextState_ = PlayerBehavior::MOVE;
        return;
    }

    // 移動キーが入力されていない
    if (isMove == false)
    {
        // PlayerState を STOOP(しゃがみ)へ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }


    // 着地している && ジャンプキーが入力された
    if (isLanding && isJumpTrigger) // ここを通っている時点で、[左SHIFT] + [移動キー]は入力されている
    {
        // コインが "kNeedCoin_"以上ある場合
        if (commonInfo_->coinNum_ >= PlayerBehavior_JumpLong::kNeedCoin_)
        {
            // 規定枚数コインを消費する
            commonInfo_->coinNum_ -= PlayerBehavior_JumpLong::kNeedCoin_;

            // PlayerState を JUMP_LONG(幅跳び)へ
            nextState_ = PlayerBehavior::JUMP_LONG;
            PostEffectManager::GetInstance()->RequestChangePostEffect(PostEffectType::RADIALBLUR, 0.4f);
            return;
        }
    }
}


void PlayerBehavior_Jump::Entry(void) // JUMP
{
    commonInfo_->jumpVecNorm_ = commonInfo_->kJumpPower_;
}

void PlayerBehavior_Jump::Execute(void)
{
    // 1フレーム前の3軸を記録
    commonInfo_->axes_old_ = commonInfo_->axes_;

    // ステート確認用
    debug_curState_ = PlayerBehavior::JUMP;

    // 入力ベクトルの取得。
    Vector2 vec2_input = Process_GetInput();
    // モデル用のaxes計算
    Process_CalculateModelAxes(vec2_input);


    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right, commonInfo_->axes_.up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)
    // 定義した値等を現在の姿勢として記録
    commonInfo_->axes_.forward = pForwardFromCamera;
    commonInfo_->axes_.right = redefinitionPRightFromCamera;
    commonInfo_->axes_.up = commonInfo_->axes_.up;


    // 重力 ※ジャンプ量に加算してる
    Process_Gravity();
    // 移動vec = (前後vec * 入力vec.y) + (水平vec * 入力vec.y)
    Vector3 moveVec = (commonInfo_->axes_.forward * vec2_input.y) + (commonInfo_->axes_.right * vec2_input.x);
    // 規定割合のコピーと0除算チェック
    float moveSpeedDived = kMove_divide_;
    if (moveSpeedDived == 0.f) moveSpeedDived = 1.f; // 0除算チェック
    // 移動量 = 移動vec * (移動速度 / 規定割合) + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * (commonInfo_->kMoveSpeed_ / moveSpeedDived)) + (commonInfo_->axes_.up * commonInfo_->jumpVecNorm_);
    // 座標更新
    Process_Transform(velocity);


    // カメラ座標用の値を補正
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;


    // 入力ベクトルが0でないなら、1フレーム前の入力ベクトルを記録
    if (vec2_input.IsNonZero()) { commonInfo_->vec2_input_old_ = vec2_input; }
#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Jump::RequirementCheck(void)
{
    //イベント中の場合、判定を行わない。
    if (commonInfo_->eventState_ != PlayerEventState::NONE) { return; }

    const bool isDown_LT = XPAD::GetLT();
    const bool isDown_LStick = (bool)(XPAD::GetLStick().Length());

    const bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    const bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);

    const bool isStoop = isDown_LT + isDown_LSHIFT;
    const bool isMove = isDown_LStick + isDown_anyWASD;
    bool isLanding = commonInfo_->isLanding_;

    // 着地している
    if (isLanding)
    {
        // 移動キーを押していない && 左SHIFTを押していない
        if (isMove == false && isStoop == false)
        {
            // PlayerState をIdleへ
            nextState_ = PlayerBehavior::IDLE;
            return;
        }

        // 移動キーを押している && 左SHIFTを押していない
        if (isMove && isStoop == false)
        {
            nextState_ = PlayerBehavior::MOVE;
            return;
        }
    }

    // 移動入力をしている && しゃがみ入力をしている && 着地している
    if (isMove && isStoop && isLanding)
    {
        nextState_ = PlayerBehavior::STOOP;
        return;
    }
}

void PlayerBehavior_JumpLong::Entry(void)
{
    commonInfo_->jumpVecNorm_ = commonInfo_->kJumpLongPower_;
    vec2_entryInput_ = Process_GetInput();
    // 運んだまま幅跳びは出来ない。
    commonInfo_->is_carrySnake_ = false;
}

void PlayerBehavior_JumpLong::Execute(void)
{
    // 1フレーム前の3軸を記録
    commonInfo_->axes_old_ = commonInfo_->axes_;

    // ステート確認用
    debug_curState_ = PlayerBehavior::JUMP_LONG;

    // 入力ベクトルは、幅跳び使用時の入力固定
    Vector2 vec2_input = vec2_entryInput_;
    // 固定とは別で入力ベクトルをとって、固定されてるベクトルに影響を与える（固定入力ベクトルによる移動を軽減する）
    Vector2 effect_input = Process_GetInput() / 2.2f;
    // 入力値0なら、1F前と同じ入力を使う。
    if (effect_input.IsNonZero() == false) { effect_input = commonInfo_->vec2_input_old_; }
    vec2_input += effect_input;          // 加算
    vec2_input = vec2_input.Normalize(); // 正規化

    // モデル用のaxes計算
    Process_CalculateModelAxes(vec2_input);

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right, commonInfo_->axes_.up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)
    // 定義した値等を現在の姿勢として記録
    commonInfo_->axes_.forward = pForwardFromCamera;
    commonInfo_->axes_.right = redefinitionPRightFromCamera;
    commonInfo_->axes_.up = commonInfo_->axes_.up;

    // 重力 ※ジャンプ量に加算してる
    Process_Gravity();
    // 移動vec = (前後vec * 入力vec.y) + (水平vec * 入力vec.y)
    Vector3 moveVec = (commonInfo_->axes_.forward * vec2_input.y) + (commonInfo_->axes_.right * vec2_input.x);
    // 移動量 = 移動vec * 幅跳び用移動速度 + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * commonInfo_->kMoveJumpLongSpeed_) + (commonInfo_->axes_.up * commonInfo_->jumpVecNorm_);
    // 座標更新
    Process_Transform(velocity);

    // カメラ座標用の値を補正
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;

    // ラジアルブラーを使用する。段々と小さくなる
    RadialBlur* radialPtr = static_cast<RadialBlur*>(PostEffectManager::GetInstance()->GetPostEffectPtr());
    radialPtr->SetBlurValue((std::max)(0.1f, radialPtr->GetBlurValue() - 0.02f));

    // 入力ベクトルが0でないなら、1フレーム前の入力ベクトルを記録 ※ここだけ、記録するのはeffect_input
    if (effect_input.IsNonZero()) { commonInfo_->vec2_input_old_ = effect_input; }

#ifdef _DEBUG
    GUI::Begin("playerBehavior_JumpLong");
    GUI::Text("vec2_entryInput_:   [%f,%f]", vec2_entryInput_.x, vec2_entryInput_.y);
    GUI::Text("effect_input:       [%f,%f]", effect_input.x, effect_input.y);
    GUI::Text("vec2_input:         [%f,%f]", vec2_input.x, vec2_input.y);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_JumpLong::RequirementCheck(void)
{
    //イベント中の場合、判定を行わない。
    if (commonInfo_->eventState_ != PlayerEventState::NONE) { return; }

    const bool isDown_LStick = (bool)(XPAD::GetLStick().Length());
    const bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));

    const bool isMove = isDown_LStick + isDown_anyWASD;
    bool isLanding = commonInfo_->isLanding_;

    // 着地している
    if (isLanding)
    {
        // 移動キーを押していない
        if (isMove == false)
        {
            // PlayerState をIdleへ
            nextState_ = PlayerBehavior::IDLE;
            PostEffectManager::GetInstance()->RequestChangePostEffect(PostEffectType::NONE);
            return;
        }

        // 移動キーを押している
        if (isMove)
        {
            nextState_ = PlayerBehavior::MOVE;
            PostEffectManager::GetInstance()->RequestChangePostEffect(PostEffectType::NONE);
            return;
        }
    }
}

IPlayerBehavior::IPlayerBehavior(Player* arg_playerPtr)
    : nextState_(PlayerBehavior::NONE), playerPtr_(arg_playerPtr)
{
    // sharedPtrで同じ情報を共有する
    commonInfo_ = playerPtr_->commonInfo_;
}

void IPlayerBehavior::CopySharedCommonInfo(const std::shared_ptr<Player_CommonInfomation>& arg_shared)
{
    commonInfo_ = arg_shared;
}

const Vector2 IPlayerBehavior::Process_GetInput(void)
{
    // 入力ベクトル
    Vector2 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec += XPAD::GetLStick();
    inputVec = inputVec.Normalize();

    return inputVec;
}

void IPlayerBehavior::Process_Gravity(void)
{
    // 重力
    const float jumpVecNorm = commonInfo_->jumpVecNorm_ - commonInfo_->kGravity_;
    commonInfo_->jumpVecNorm_ = jumpVecNorm;
}

void IPlayerBehavior::Process_Transform(const Vector3& arg_velocity)
{
    // どれだけ移動したのかを記録
    commonInfo_->velocity_ = arg_velocity;

    // 新規座標
    const Vector3 position = commonInfo_->transform_.position + arg_velocity;
    // 座標を更新
    commonInfo_->transform_.position = position;
}

void IPlayerBehavior::Process_CalculateModelAxes(const Vector2& arg_input)
{
    Vector2 input = arg_input;
    if (input.IsNonZero() == false) { input = commonInfo_->vec2_input_old_; }

    // なぜか、プレイヤーのモデルは常に画面に対して上に向いてるので、そこに仮想の上ベクトルと角度を取って、
    // モデルを上ベクトルを軸に回転させる。モデルは、入力ベクトルの方向を向くようになっているが、ここの処理は移動入力をしたときのみ通る（ステートパターン）
    const float radian_rotate2 = std::acosf(Math::Vec2::Dot({ 0,1 }, input));
    float radian_rotate{};
    radian_rotate = radian_rotate2;
    if (input.x < 0)
    {
        radian_rotate = 3.1415926535f + (3.1415926535f - radian_rotate2);
    }

    // プレイヤーの3つの軸をコピー
    const Axis3& pAxes = commonInfo_->axes_;
    // プレイヤーの上ベクトルを軸として、rot_total分だけ、回転させるクオータニオンを生成
    Quaternion rot_quaternion = Math::QuaternionF::MakeAxisAngle(pAxes.up, radian_rotate);
    // クオータニオンを使って、正面ベクトルと右ベクトルを
    const Vector3 vec_rotatedForward = Math::QuaternionF::RotateVector(pAxes.forward, rot_quaternion);
    const Vector3 vec_rotatedRight = Math::QuaternionF::RotateVector(pAxes.right, rot_quaternion);

    const Axis3 modelAxes{ vec_rotatedForward,vec_rotatedRight,pAxes.up };
    commonInfo_->axes_4model_ = modelAxes;
}
