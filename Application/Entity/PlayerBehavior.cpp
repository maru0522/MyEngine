#include "PlayerBehavior.h"
#include "Input.h"
#include "Vector3.h"
#include "CameraManager.h"
#include "Player.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

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
    if (arg_state == PlayerBehavior::JUMP_LONG) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }

    return std::unique_ptr<IPlayerBehavior>();
}

//----------------------------------------------------------------------------------------
PlayerBehaviorMachine::PlayerBehaviorMachine(Player* arg_playerPtr, PlayerBehavior arg_state)
    : playerPtr_(arg_playerPtr)
{
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
//IDLE,       //
//STOOP,      // しゃがみ
//
//MOVE,       // 移動
//MOVE_STOOP, // しゃがみ移動
//
//JUMP,       // ジャンプ
//JUMP_STOOP, // じゃがみジャンプ
//JUMP_REVERSE,  // 反転ジャンプ
//JUMP_LONG,  // 幅跳び

void PlayerBehavior_Idle::Execute(void) // "IDLE"
{
    debug_curState_ = PlayerBehavior::IDLE;

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 移動量 = 上方向 * ジャンプ量
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle状態は重力以外の移動量は発生しない想定

    // 座標更新
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);

    // 姿勢制御
    {
        //// 現在のプレイヤーの各軸情報
        //const Axis3& playerAxes = GetPlayerAxes();

        //// 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
        //Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
        //Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
        //SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
    }
}

void PlayerBehavior_Idle::RequirementCheck(void)
{
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    bool isLanding = GetPlayerJumpVecNorm() == 0.f;

    // 左シフトが入力されている
    if (isDown_LSHIFT)
    {
        // PlayerState を STOOP(しゃがみ)へ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }

    // 移動キーが入力されている
    if (isDown_anyWASD)
    {
        // PlayerState を MOVE(移動)へ
        nextState_ = PlayerBehavior::MOVE;
        return;
    }

    // SPACEが入力されている && 地面に足がついている
    if (isDown_SPACE && isLanding)
    {
        // PlayerState を JUMP(ジャンプ)へ
        nextState_ = PlayerBehavior::JUMP;
        return;
    }
}

void PlayerBehavior_Stoop::Execute(void) // "STOOP"
{
    debug_curState_ = PlayerBehavior::STOOP;

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 移動量 = 上方向 * ジャンプ量
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle状態は重力以外の移動量は発生しない想定

    // 座標更新
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Stoop::RequirementCheck(void)
{
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    bool isLanding = GetPlayerJumpVecNorm() == 0.f;

    // 左SHIFTが入力されていない && 移動キーが入力されていない && SPACEが入力されている
    if (isDown_LSHIFT == false && isDown_anyWASD == false && isDown_SPACE == false)
    {
        // PlayerState を Idleへ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 左SHIFTが入力されている && 移動キーが入力されている
    if (isDown_anyWASD) // ここを通っている時点で、実質左SHIFTが入力されている
    {
        // PlayerState を MOVE_STOOP(しゃがみ移動)へ
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
    }

    // SPACEが入力されている && 地面に足がついている
    if (isDown_SPACE && isLanding)
    {
        // PlayerState を JUMP_STOOP(しゃがみジャンプ)へ
        nextState_ = PlayerBehavior::JUMP_STOOP;
        return;
    }
}

void PlayerBehavior_Move::Execute(void) // "MOVE"
{
    debug_curState_ = PlayerBehavior::MOVE;

    // 入力ベクトル
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec = inputVec.Normalize();

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->right, GetPlayerAxes().up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(GetPlayerAxes().up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)

    // 移動ベクトル = 前後vec + 水平vec
    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);

    // カメラ座標用の値を補正
    {
        if (GetPlayerJumpVecNorm())
        {
            // カメラとプレイヤーの距離
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // ジャンプ時にカメラの追従が軽減 ≒ 画面の揺れを抑制する目的
            // 内積が規定値未満の時ジャンプを繰り返すとカメラ距離どんどん遠くなっていく不具合が出てる
            SetPlayerCurrentRad(dist);
        }

        // プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
        // 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
        {
            // カメラとプレイヤーの距離
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // 該当距離が、本来設定されているプレイヤーとの距離より短い場合、該当距離を設定距離とする。
            if (dist < GetPlayerCurrentRad())
            {
                // プレイヤーがカメラ側に向かって移動する際、カメラの座標を固定する意図
                // しかし、現状だとカメラが遠ざかる処理が上手く機能していない為コメントアウト。
                //current_rad_ = dist;
            }
        }
        else
        {
            // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
            if (GetPlayerCurrentRad() < GetPlayerDefaultRad())
            {
                SetPlayerCurrentRad(GetPlayerCurrentRad() + 0.1f);
                //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
            }
            else if (GetPlayerCurrentRad() > GetPlayerDefaultRad())
            {
                SetPlayerCurrentRad(GetPlayerCurrentRad() - 0.1f);
            }

            float theta = GetPlayerTheta();
            float phi = GetPlayerPhi();

            theta += 0.02f * inputVec.y;
            phi += 0.02f * inputVec.x;
            Math::Function::Loop(theta, 0.f, 6.28319f);
            Math::Function::Loop(phi, 0.f, 6.28319f);

            SetPlayerTheta(theta);
            SetPlayerPhi(phi);
        }
    }

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 移動量 = 移動vec * 移動速度 + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveSpeed()) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

    // 座標更新
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
    SetPlayerMoveVec(moveVec);

    // 姿勢制御
    {
        // 現在のプレイヤーの各軸情報
        const Axis3& playerAxes = GetPlayerAxes();

        // 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
        // 移動入力があった場合
        if (moveVec.IsNonZero())
        {
            // 移動方向を向くような、移動方向に合わせた姿勢にするために右向きベクトルを再計算
            Vector3 upFromAxis = playerAxes.up; // 上ベクトル：(更新された上ベクトルを取得）
            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 右ベクトル：(更新された上ベクトル x 移動ベクトル（移動方向 ≒ 正面ベクトル))
            SetPlayerAxes({ moveVec.Normalize(),rightFromMoveVec.Normalize(), playerAxes.up });
        }
    }

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Move::RequirementCheck(void)
{
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    bool isLanding = GetPlayerJumpVecNorm() == 0.f;

    // 左SHIFTが入力されていない && 移動キーが入力されていない && SPACEが入力されている
    if (isDown_LSHIFT == false && isDown_anyWASD == false && isDown_SPACE == false)
    {
        // PlayerState を IDLEへ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 左シフトが入力されている
    if (isDown_LSHIFT) // ここを通っている時点で、実質移動キーが入力されている
    {
        // PlayerState を MOVE_STOOP(しゃがみ移動)へ
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
    }

    // SPACEが入力されている && 地面に足がついている
    if (isDown_SPACE && isLanding)
    {
        // PlayerState を JUMP(ジャンプ)へ
        nextState_ = PlayerBehavior::JUMP;
        return;
    }
}

void PlayerBehavior_MoveStoop::Execute(void)
{
    debug_curState_ = PlayerBehavior::MOVE_STOOP;

    // 入力ベクトル
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec = inputVec.Normalize();

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->right, GetPlayerAxes().up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(GetPlayerAxes().up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)

    // 移動ベクトル = 前後vec + 水平vec
    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);

    // カメラ座標用の値を補正
    {
        if (GetPlayerJumpVecNorm())
        {
            // カメラとプレイヤーの距離
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // ジャンプ時にカメラの追従が軽減 ≒ 画面の揺れを抑制する目的
            // 内積が規定値未満の時ジャンプを繰り返すとカメラ距離どんどん遠くなっていく不具合が出てる
            SetPlayerCurrentRad(dist);
        }

        // プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
        // 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
        {
            // カメラとプレイヤーの距離
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // 該当距離が、本来設定されているプレイヤーとの距離より短い場合、該当距離を設定距離とする。
            if (dist < GetPlayerCurrentRad())
            {
                // プレイヤーがカメラ側に向かって移動する際、カメラの座標を固定する意図
                // しかし、現状だとカメラが遠ざかる処理が上手く機能していない為コメントアウト。
                //current_rad_ = dist;
            }
        }
        else
        {
            // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
            if (GetPlayerCurrentRad() < GetPlayerDefaultRad())
            {
                SetPlayerCurrentRad(GetPlayerCurrentRad() + 0.1f);
                //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
            }
            else if (GetPlayerCurrentRad() > GetPlayerDefaultRad())
            {
                SetPlayerCurrentRad(GetPlayerCurrentRad() - 0.1f);
            }

            float theta = GetPlayerTheta();
            float phi = GetPlayerPhi();

            theta += 0.02f * inputVec.y;
            phi += 0.02f * inputVec.x;
            Math::Function::Loop(theta, 0.f, 6.28319f);
            Math::Function::Loop(phi, 0.f, 6.28319f);

            SetPlayerTheta(theta);
            SetPlayerPhi(phi);
        }
    }

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 移動量 = 移動vec * 移動速度 + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * (GetPlayerMoveSpeed() / 2)) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

    // 座標更新
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
    SetPlayerMoveVec(moveVec);

    // 姿勢制御
    {
        // 現在のプレイヤーの各軸情報
        const Axis3& playerAxes = GetPlayerAxes();

        // 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
        // 移動入力があった場合
        if (moveVec.IsNonZero())
        {
            // 移動方向を向くような、移動方向に合わせた姿勢にするために右向きベクトルを再計算
            Vector3 upFromAxis = playerAxes.up; // 上ベクトル：(更新された上ベクトルを取得）
            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 右ベクトル：(更新された上ベクトル x 移動ベクトル（移動方向 ≒ 正面ベクトル))
            SetPlayerAxes({ moveVec.Normalize(),rightFromMoveVec.Normalize(), playerAxes.up });
        }
    }

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_MoveStoop::RequirementCheck(void)
{
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);

    // 左SHIFTが入力されていない && 移動キーが入力されていない && SPACEが入力されている
    if (isDown_LSHIFT == false && isDown_anyWASD == false  && isDown_SPACE == false)
    {
        // PlayerState を Idleへ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 左SHIFTが入力されていない
    if (isDown_LSHIFT == false)
    {
        // PlayerState を MOVE(移動)へ
        nextState_ = PlayerBehavior::MOVE;
        return;
    }

    // 移動キーが入力されていない
    if (isDown_anyWASD == false)
    {
        // PlayerState を STOOP(しゃがみ)へ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }



    // 移動キーが入力されている
    if (isDown_SPACE) // ここを通っている時点で、実質左SHIFTが入力されている
    {
        // PlayerState を JUMP_LONG(幅跳び)へ
        nextState_ = PlayerBehavior::JUMP_LONG;
        return;
    }
}

void PlayerBehavior_Jump::Entry(void) // JUMP
{
    SetPlayerJumpVecNorm(GetPlayerJumpPower());
}

void PlayerBehavior_Jump::Execute(void)
{
    debug_curState_ = PlayerBehavior::JUMP;

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 移動量 = 上方向 * ジャンプ量
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle状態は重力以外の移動量は発生しない想定

    // 座標更新
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
}

void PlayerBehavior_Jump::RequirementCheck(void)
{
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    //bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    //bool isLanding = GetPlayerJumpVecNorm() == 0.f;

    // 移動キーが入力されていない && 左SHIFTが入力されていない
    if (isDown_anyWASD == false && isDown_LSHIFT == false)
    {
        // PlayerState を IDLEへ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 移動キーが入力されている && 左SHIFTが入力されていない
    if (KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S) != 0 &&
        KEYS::IsDown(DIK_LSHIFT) == false)
    {
        nextState_ = PlayerBehavior::MOVE;
        return;
    }

    // 移動キーが入力されている && 左SHIFTが入力されている
    if (KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S) != 0 &&
        KEYS::IsDown(DIK_LSHIFT))
    {
        nextState_ = PlayerBehavior::STOOP;
        return;
    }
}

void PlayerBehavior_JumpLong::Entry(void)
{
    SetPlayerJumpVecNorm(GetPlayerJumpPower() / 3);
}

void PlayerBehavior_JumpLong::Execute(void)
{
    debug_curState_ = PlayerBehavior::JUMP_LONG;


    // 幅跳びする方向ベクトル
    Vector3 vec3_jumpLong = GetPlayerMoveVec();

    // 入力ベクトル
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec = inputVec.Normalize();

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->right, GetPlayerAxes().up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(GetPlayerAxes().up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)

    // 移動ベクトル = 前後vec + 水平vec
    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);

    // カメラ座標用の値を補正
    {
        if (GetPlayerJumpVecNorm())
        {
            // カメラとプレイヤーの距離
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // ジャンプ時にカメラの追従が軽減 ≒ 画面の揺れを抑制する目的
            // 内積が規定値未満の時ジャンプを繰り返すとカメラ距離どんどん遠くなっていく不具合が出てる
            SetPlayerCurrentRad(dist);
        }

        // プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
        // 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
        {
            // カメラとプレイヤーの距離
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // 該当距離が、本来設定されているプレイヤーとの距離より短い場合、該当距離を設定距離とする。
            if (dist < GetPlayerCurrentRad())
            {
                // プレイヤーがカメラ側に向かって移動する際、カメラの座標を固定する意図
                // しかし、現状だとカメラが遠ざかる処理が上手く機能していない為コメントアウト。
                //current_rad_ = dist;
            }
        }
        else
        {
            // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
            if (GetPlayerCurrentRad() < GetPlayerDefaultRad())
            {
                SetPlayerCurrentRad(GetPlayerCurrentRad() + 0.1f);
                //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
            }
            else if (GetPlayerCurrentRad() > GetPlayerDefaultRad())
            {
                SetPlayerCurrentRad(GetPlayerCurrentRad() - 0.1f);
            }

            float theta = GetPlayerTheta();
            float phi = GetPlayerPhi();

            theta += 0.02f * inputVec.y;
            phi += 0.02f * inputVec.x;
            Math::Function::Loop(theta, 0.f, 6.28319f);
            Math::Function::Loop(phi, 0.f, 6.28319f);

            SetPlayerTheta(theta);
            SetPlayerPhi(phi);
        }
    }

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 移動量 = 移動vec * 移動速度 + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveSpeed()) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

    // 座標更新
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
    SetPlayerMoveVec(moveVec);

    // 姿勢制御
    {
        // 現在のプレイヤーの各軸情報
        const Axis3& playerAxes = GetPlayerAxes();

        // 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
        // 移動入力があった場合
        if (moveVec.IsNonZero())
        {
            // 移動方向を向くような、移動方向に合わせた姿勢にするために右向きベクトルを再計算
            Vector3 upFromAxis = playerAxes.up; // 上ベクトル：(更新された上ベクトルを取得）
            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 右ベクトル：(更新された上ベクトル x 移動ベクトル（移動方向 ≒ 正面ベクトル))
            SetPlayerAxes({ moveVec.Normalize(),rightFromMoveVec.Normalize(), playerAxes.up });
        }
    }

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_JumpLong::RequirementCheck(void)
{
}

//----------------------------------------------------------------------------------------
const Vector3& IPlayerBehavior::GetPlayerMoveVec(void)
{
    return playerPtr_->moveVec_;
}

const Vector3& IPlayerBehavior::GetPlayerVelociy(void)
{
    return playerPtr_->velocity_;
}

const Axis3& IPlayerBehavior::GetPlayerAxes(void)
{
    return playerPtr_->axes_;
}

const Transform& IPlayerBehavior::GetPlayerTransform(void)
{
    return playerPtr_->transform_;
}

float IPlayerBehavior::GetPlayerJumpVecNorm(void)
{
    return playerPtr_->jumpVecNorm_;
}

float IPlayerBehavior::GetPlayerDefaultRad(void)
{
    return playerPtr_->default_rad_;
}

float IPlayerBehavior::GetPlayerCurrentRad(void)
{
    return playerPtr_->current_rad_;
}

float IPlayerBehavior::GetPlayerTheta(void)
{
    return playerPtr_->theta_;
}

float IPlayerBehavior::GetPlayerPhi(void)
{
    return playerPtr_->phi_;
}

float IPlayerBehavior::GetPlayerGravity(void)
{
    return playerPtr_->kGravity_;
}

float IPlayerBehavior::GetPlayerJumpPower(void)
{
    return playerPtr_->kJumpPower_;
}

float IPlayerBehavior::GetPlayerJumpLongPower(void)
{
    return playerPtr_->kJumpLongPower_;
}

float IPlayerBehavior::GetPlayerMoveSpeed(void)
{
    return playerPtr_->kMoveSpeed_;
}

CameraManager* IPlayerBehavior::GetPlayerCamMPtr(void)
{
    return playerPtr_->camMPtr_;
}

void IPlayerBehavior::SetPlayerMoveVec(const Vector3& arg_moveVec)
{
    playerPtr_->moveVec_ = arg_moveVec;
}

void IPlayerBehavior::SetPlayerVelocity(const Vector3& arg_velocity)
{
    playerPtr_->moveVec_ = arg_velocity;
}

void IPlayerBehavior::SetPlayerAxes(const Axis3& arg_axes)
{
    playerPtr_->axes_ = arg_axes;
}

void IPlayerBehavior::SetPlayerJumpVecNorm(float arg_jumpVecNorm)
{
    playerPtr_->jumpVecNorm_ = arg_jumpVecNorm;
}

void IPlayerBehavior::SetPlayerCurrentRad(float arg_currentRad)
{
    playerPtr_->current_rad_ = arg_currentRad;
}

void IPlayerBehavior::SetPlayerTheta(float arg_theta)
{
    playerPtr_->theta_ = arg_theta;
}

void IPlayerBehavior::SetPlayerPhi(float arg_phi)
{
    playerPtr_->phi_ = arg_phi;
}

void IPlayerBehavior::SetPlayerTransformPosition(const Vector3& arg_pos)
{
    playerPtr_->transform_.position = arg_pos;
}
