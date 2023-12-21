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
    debug_curState_ = PlayerBehavior::IDLE;

    // 重力
    Process_Gravity();

    // 移動量 = 上方向 * ジャンプ量
    const Vector3 velocity = commonInfo_->axes_.up * commonInfo_->jumpVecNorm_; // Idle状態は重力以外の移動量は発生しない想定
    Process_Transform(velocity);

    // 姿勢制御
    {
        //// 各軸全て、初期値（姿勢が初期状態）かどうか。
        //const bool is_default = !commonInfo_->axes_old_.forward.IsNonZero() && !commonInfo_->axes_old_.right.IsNonZero() && !commonInfo_->axes_old_.up.IsNonZero();
        //// 初期状態ならスキップ
        //if (is_default) { return; }


        //const float deg = 15.f;
        //const float rad = Math::Function::ToRadian(deg);
        //// "deg"度分の内積値
        //const float dot_deg = std::cosf(rad);
        //// 1F前の上ベクトルと新規上ベクトルの内積値
        //const float dot_up = Math::Vec3::Dot(commonInfo_->axes_old_.up, commonInfo_->vec3_newUp_);
        //// 内積値が、"dot_deg"の値より大きいかどうか
        //const bool is_bigger = dot_up > dot_deg;
        //// 小さいならスキップ
        //if (is_bigger == false) { return; }

        //// 姿勢を再計算
        //Axis3 axes{};
        //// 新規上ベクトル x 現在の正面ベクトル = 新規右ベクトル ※新規上ベクトルは、Player.cppのOnCollision内で毎フレーム定義されている
        //Vector3 newRight = Math::Vec3::Cross(commonInfo_->vec3_newUp_, commonInfo_->axes_.forward).Normalize();
        //axes.right = newRight;
        //// 新規右ベクトル x 新規上ベクトル = 新規正面ベクトル
        //Vector3 newForward = Math::Vec3::Cross(newRight, commonInfo_->vec3_newUp_).Normalize();
        //axes.forward = newForward;
        //// 新規姿勢の上ベクトルを代入
        //axes.up = commonInfo_->vec3_newUp_;

        //// 1F前として記録
        //commonInfo_->axes_old_ = commonInfo_->axes_;
        //// 現在の姿勢として設定。
        //commonInfo_->axes_ = axes;
    }

    // カメラ座標用の値を補正
    {
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
}

void PlayerBehavior_Idle::RequirementCheck(void)
{
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
    if (isStoop)
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

void PlayerBehavior_Stoop::Execute(void) // "STOOP"
{
    debug_curState_ = PlayerBehavior::STOOP;

    // 重力
    Process_Gravity();

    // 移動量 = 上方向 * ジャンプ量
    const Vector3 velocity = commonInfo_->axes_.up * commonInfo_->jumpVecNorm_; // stoop状態は重力以外の移動量は発生しない想定（ただその場でしゃがんでるだけだから）
    Process_Transform(velocity);

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Stoop::RequirementCheck(void)
{
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
        // PlayerState を MOVE_STOOP(しゃがみ移動)へ
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
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
    {
        // なぜか、プレイヤーのモデルは常に画面に対して上に向いてるので、そこに仮想の上ベクトルと角度を取って、
        // モデルを上ベクトルを軸に回転させる。モデルは、入力ベクトルの方向を向くようになっているが、ここの処理は移動入力をしたときのみ通る（ステートパターン）
        const float radian_rotate2 = std::acosf(Math::Vec2::Dot({ 0,1 }, vec2_input));
        float radian_rotate{};
        radian_rotate = radian_rotate2;
        if (vec2_input.x < 0)
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

#ifdef _DEBUG
        GUI::Begin("Debug_Move");
        GUI::Text("inputVec : %f,%f", inputVec.x, inputVec.y);
        GUI::Text("radian : %f", radian_rotate);
        GUI::End();
#endif // DEBUG
    }


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
    // 移動量 = 移動vec * 移動速度 + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * commonInfo_->kMoveSpeed_) + (commonInfo_->axes_.up * commonInfo_->jumpVecNorm_);
    // 座標更新
    Process_Transform(velocity);


    // カメラ座標用の値を補正
    ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }
    BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
    ptr_cam_behind->axes_player_ = commonInfo_->axes_;
    ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;


    // 1フレーム前の入力ベクトルを記録
    commonInfo_->vec2_input_old_ = vec2_input;
#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Move::RequirementCheck(void)
{
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

    // 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・&& 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・↑縺・&& SPACE縺悟・蜉帙＆繧後※縺・ｋ
    if (isStoop == false && isMove == false && isJump == false)
    {
        // PlayerState 繧・IDLE縺ｸ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 蟾ｦ繧ｷ繝輔ヨ縺悟・蜉帙＆繧後※縺・ｋ
    if (isStoop) // 縺薙％繧帝壹▲縺ｦ縺・ｋ譎らせ縺ｧ縲∝ｮ溯ｳｪ遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ
    {
        // PlayerState 繧・MOVE_STOOP(縺励ｃ縺後∩遘ｻ蜍・縺ｸ
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
    }

    // SPACE縺悟・蜉帙＆繧後※縺・ｋ && 蝨ｰ髱｢縺ｫ雜ｳ縺後▽縺・※縺・ｋ
    if (isJump && isLanding)
    {
        // PlayerState 繧・JUMP(繧ｸ繝｣繝ｳ繝・縺ｸ
        nextState_ = PlayerBehavior::JUMP;
        return;
    }
}

void PlayerBehavior_MoveStoop::Execute(void)
{
    debug_curState_ = PlayerBehavior::MOVE_STOOP;

    // 蜈･蜉帙・繧ｯ繝医Ν
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec += Vector3(XPAD::GetLStick().x, XPAD::GetLStick().y, 0.f);
    inputVec = inputVec.Normalize();

    // 繧ｫ繝｡繝ｩ隕也せ縺ｮ繝励Ξ繧､繝､繝ｼ遘ｻ蜍輔・繧ｯ繝医Ν
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right, commonInfo_->axes_.up).Normalize(); // 豁｣髱｢Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up, pForwardFromCamera).Normalize(); // 蜿ｳVec: cross(p.upVec, pForwardFromCamera)

    // 遘ｻ蜍輔・繧ｯ繝医Ν = 蜑榊ｾ計ec + 豌ｴ蟷ｳvec
    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);

    //    // カメラ座標用の値を補正
    //    {
    //        if (commonInfo_->jumpVecNorm_)
    //        {
    //            // カメラとプレイヤーの距離
    //            float dist = (commonInfo_->camMPtr_->GetCurrentCamera()->GetTransformMatrix().GetMatPos() - commonInfo_->transform_.position).Length();
    //
    //            // ジャンプ時にカメラの追従が軽減 ≒ 画面の揺れを抑制する目的
    //            // 内積が規定値未満の時ジャンプを繰り返すとカメラ距離どんどん遠くなっていく不具合が出てる
    //            commonInfo_->current_rad_ = dist;
    //        }
    //
    //
    //        ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
    //        if (ptr_cam->GetId().starts_with("SphericalCamera_") == false) { return; }
    //        SphericalCamera* ptr_cam_spherical = static_cast<SphericalCamera*>(ptr_cam);
    //        Vector3 vec_sphericalEye = Vector3(GetPlayerTransform().position - ptr_cam_spherical->GetTransform().position).Normalize();
    //        ptr_cam_spherical->Debug_need(GetPlayerDefaultRad(), GetPlayerTransform().position, GetPlayerTransform().position);
    //
    //        float theta = ptr_cam_spherical->theta_;
    //        float phi = ptr_cam_spherical->phi_;
    //        float psi = ptr_cam_spherical->psi_;
    //
    //        // プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
    //        // 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
    //        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3().forward) < 0.7f)
    //        {
    //
    //            if (std::fabsf(GetPlayerAxes().right.Dot(vec_sphericalEye) < 0.6f))
    //            {
    //                theta += 0.005f * inputVec.x;
    //                phi += 0.015f * inputVec.x;
    //            }
    //        }
    //        else
    //        {
    //            // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
    //            if (GetPlayerCurrentRad() < GetPlayerDefaultRad())
    //            {
    //                SetPlayerCurrentRad(GetPlayerCurrentRad() + 0.1f);
    //                //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
    //            }
    //            else if (GetPlayerCurrentRad() > GetPlayerDefaultRad())
    //            {
    //                SetPlayerCurrentRad(GetPlayerCurrentRad() - 0.1f);
    //            }
    //
    //            theta += 0.02f * inputVec.y;
    //            phi += 0.02f * inputVec.x;
    //        }
    //
    //        Math::Function::Loop(theta, 0.f, 6.28319f);
    //        Math::Function::Loop(phi, 0.f, 6.28319f);
    //        Math::Function::Loop(psi, 0.f, 6.28319f);
    //        ptr_cam_spherical->SetSphericalRotate(theta, phi, psi);
    //    }
    //
    //    // 驥榊鴨
    //    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());
    //
    //    // 遘ｻ蜍暮㍼ = 遘ｻ蜍夫ec * 遘ｻ蜍暮溷ｺｦ + 荳頑婿蜷・* 繧ｸ繝｣繝ｳ繝鈴㍼
    //    Vector3 velocity = (moveVec.Normalize() * (GetPlayerMoveSpeed() / 2)) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());
    //
    //    // 蠎ｧ讓呎峩譁ｰ
    //    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    //    SetPlayerVelocity(velocity);
    //    SetPlayerMoveVec(moveVec);
    //
    //    // 蟋ｿ蜍｢蛻ｶ蠕｡
    //    {
    //        // 迴ｾ蝨ｨ縺ｮ繝励Ξ繧､繝､繝ｼ縺ｮ蜷・ｻｸ諠・ｱ
    //        const Axis3& playerAxes = GetPlayerAxes();
    //
    //        // 逅・擇縺ｮ縺ｩ縺ｮ菴咲ｽｮ縺ｫ縺・ｋ縺九↓蠢懊§縺ｦ縲∵ｭ｣縺励＞蟋ｿ蜍｢縺ｫ縺吶ｋ縺溘ａ縺ｫ3霆ｸ繧貞・險育ｮ・
    //        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 蜿ｳ繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν x 蜿､縺・ｭ｣髱｢繝吶け繝医Ν)
    //        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 豁｣髱｢繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺溷承繝吶け繝医Ν x 譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν)
    //        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
    //        // 遘ｻ蜍募・蜉帙′縺ゅ▲縺溷ｴ蜷・
    //        if (moveVec.IsNonZero())
    //        {
    //            // 遘ｻ蜍墓婿蜷代ｒ蜷代￥繧医≧縺ｪ縲∫ｧｻ蜍墓婿蜷代↓蜷医ｏ縺帙◆蟋ｿ蜍｢縺ｫ縺吶ｋ縺溘ａ縺ｫ蜿ｳ蜷代″繝吶け繝医Ν繧貞・險育ｮ・
    //            Vector3 upFromAxis = playerAxes.up; // 荳翫・繧ｯ繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν繧貞叙蠕暦ｼ・
    //            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 蜿ｳ繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν x 遘ｻ蜍輔・繧ｯ繝医Ν・育ｧｻ蜍墓婿蜷・竕・豁｣髱｢繝吶け繝医Ν))
    //            SetPlayerAxes({ moveVec.Normalize(),rightFromMoveVec.Normalize(), playerAxes.up });
    //        }
    //    }
    //
    //#ifdef _DEBUG
    //    GUI::Begin("player");
    //    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    //    GUI::End();
    //#endif // _DEBUG
}

void PlayerBehavior_MoveStoop::RequirementCheck(void)
{
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

    // 移動キーが入力されていない
    if (isMove == false)
    {
        // PlayerState を STOOP(しゃがみ)へ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }



    // 着地している && 移動キーが入力された
    if (isLanding && isJumpTrigger) // ここを通っている時点で、実質左SHIFTが入力されている
    {
        // PlayerState を JUMP_LONG(幅跳び)へ
        nextState_ = PlayerBehavior::JUMP_LONG;
        PostEffectManager::GetInstance()->RequestChangePostEffect(PostEffectType::RADIALBLUR, 0.4f);
        return;
    }
}

void PlayerBehavior_Jump::Entry(void) // JUMP
{
    commonInfo_->jumpVecNorm_ = commonInfo_->kJumpPower_;
}

void PlayerBehavior_Jump::Execute(void)
{
    debug_curState_ = PlayerBehavior::JUMP;

    // 入力ベクトル
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec += Vector3(XPAD::GetLStick().x, XPAD::GetLStick().y, 0.f);
    inputVec = inputVec.Normalize();

    // カメラ視点のプレイヤー移動ベクトル
    Vector3 pForwardFromCamera = Math::Vec3::Cross(commonInfo_->camMPtr_->GetCurrentCamera()->GetAxis3().right, commonInfo_->axes_.up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(commonInfo_->axes_.up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)

    commonInfo_->axes_.forward = pForwardFromCamera;
    commonInfo_->axes_.right = redefinitionPRightFromCamera;
    commonInfo_->axes_.up = commonInfo_->axes_.up;

    // 移動ベクトル = 前後vec + 水平vec
    Vector3 moveVec = (commonInfo_->axes_.forward * inputVec.y) + (commonInfo_->axes_.right * inputVec.x);

    // カメラ座標用の値を補正
    {
        ICamera* ptr_cam = commonInfo_->camMPtr_->GetCurrentCamera();
        if (ptr_cam->GetId().starts_with("BehindCamera_") == false) { return; }
        BehindCamera* ptr_cam_behind = static_cast<BehindCamera*>(ptr_cam);
        ptr_cam_behind->axes_player_ = commonInfo_->axes_;
        ptr_cam_behind->pos_player_ = commonInfo_->transform_.position;
    }

    // 重力
    Process_Gravity();

    float moveSpeedDived = kMove_divide_;
    if (moveSpeedDived == 0.f) moveSpeedDived = 1.f; // 0除算チェック

    // 移動量 = 移動vec * (移動速度 / 規定割合) + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * (commonInfo_->kMoveSpeed_ / moveSpeedDived)) + (commonInfo_->axes_.up * commonInfo_->jumpVecNorm_);
    // 座標更新
    Process_Transform(velocity);

    // 姿勢制御
    {
        // 現在のプレイヤーの各軸情報
        const Axis3& playerAxes = commonInfo_->axes_;

        // 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)

        commonInfo_->axes_ = { forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up };
    }

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Jump::RequirementCheck(void)
{
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

    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ && 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・ｋ
    if (isMove && isStoop)
    {
        nextState_ = PlayerBehavior::STOOP;
        return;
    }
}

void PlayerBehavior_JumpLong::Entry(void)
{
    commonInfo_->jumpVecNorm_ = commonInfo_->kJumpLongPower_;
}

void PlayerBehavior_JumpLong::Execute(void)
{
    //    debug_curState_ = PlayerBehavior::JUMP_LONG;
    //
    //    // 入力ベクトル
    //    Vector3 inputVec{};
    //    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    //    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    //    inputVec += Vector3(XPAD::GetLStick().x, XPAD::GetLStick().y, 0.f);
    //    inputVec = inputVec.Normalize();
    //
    //    // カメラ視点のプレイヤー移動ベクトル
    //    Vector3 pForwardFromCamera = Math::Vec3::Cross(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3().right, GetPlayerAxes().up).Normalize(); // 正面Vec: cross(camera.rightVec, p.upVec)
    //    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(GetPlayerAxes().up, pForwardFromCamera).Normalize(); // 右Vec: cross(p.upVec, pForwardFromCamera)
    //
    //    // 移動ベクトル = 前後vec + 水平vec
    //    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);
    //
    //    // カメラ座標用の値を補正
    //    {
    //        if (GetPlayerJumpVecNorm())
    //        {
    //            // カメラとプレイヤーの距離
    //            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetTransformMatrix().GetMatPos() - GetPlayerTransform().position).Length();
    //
    //            // ジャンプ時にカメラの追従が軽減 ≒ 画面の揺れを抑制する目的
    //            // 内積が規定値未満の時ジャンプを繰り返すとカメラ距離どんどん遠くなっていく不具合が出てる
    //            SetPlayerCurrentRad(dist);
    //        }
    //
    //
    //        ICamera* ptr_cam = GetPlayerCamMPtr()->GetCurrentCamera();
    //        if (ptr_cam->GetId().starts_with("SphericalCamera_") == false) { return; }
    //        SphericalCamera* ptr_cam_spherical = static_cast<SphericalCamera*>(ptr_cam);
    //        Vector3 vec_sphericalEye = Vector3(GetPlayerTransform().position - ptr_cam_spherical->GetTransform().position).Normalize();
    //        ptr_cam_spherical->Debug_need(GetPlayerDefaultRad(), GetPlayerTransform().position, GetPlayerTransform().position);
    //
    //        float theta = ptr_cam_spherical->theta_;
    //        float phi = ptr_cam_spherical->phi_;
    //        float psi = ptr_cam_spherical->psi_;
    //
    //        // プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
    //        // 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
    //        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3().forward) < 0.7f)
    //        {
    //
    //            if (std::fabsf(GetPlayerAxes().right.Dot(vec_sphericalEye) < 0.6f))
    //            {
    //
    //                theta += 0.005f * inputVec.x;
    //                phi += 0.015f * inputVec.x;
    //            }
    //        }
    //        else
    //        {
    //            // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
    //            if (GetPlayerCurrentRad() < GetPlayerDefaultRad())
    //            {
    //                SetPlayerCurrentRad(GetPlayerCurrentRad() + 0.1f);
    //                //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
    //            }
    //            else if (GetPlayerCurrentRad() > GetPlayerDefaultRad())
    //            {
    //                SetPlayerCurrentRad(GetPlayerCurrentRad() - 0.1f);
    //            }
    //
    //            theta += 0.02f * inputVec.y;
    //            phi += 0.02f * inputVec.x;
    //        }
    //        Math::Function::Loop(theta, 0.f, 6.28319f);
    //        Math::Function::Loop(phi, 0.f, 6.28319f);
    //        Math::Function::Loop(psi, 0.f, 6.28319f);
    //        ptr_cam_spherical->SetSphericalRotate(theta, phi, psi);
    //    }
    //
    //    // 重力
    //    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());
    //
    //    //// 移動量 = 移動vec * (移動速度 / 規定割合) + 上方向 * ジャンプ量
    //    //Vector3 velocity = ((habatobiVec.Normalize() * (GetPlayerMoveSpeed() + 5)) - (inputMoveVec.Normalize() * GetPlayerMoveSpeed())) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());
    //
    //    // 移動量 = 移動vec * (移動速度 + 5) + 上方向 * ジャンプ量
    //    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveJumpLongSpeed()) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());
    //
    //
    //    // 座標更新
    //    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    //    SetPlayerVelocity(velocity);
    //    SetPlayerMoveVec(moveVec);
    //
    //    // 姿勢制御
    //    {
    //        // 現在のプレイヤーの各軸情報
    //        const Axis3& playerAxes = GetPlayerAxes();
    //
    //        // 球面のどの位置にいるかに応じて、正しい姿勢にするために3軸を再計算
    //        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 右ベクトル：(更新された上ベクトル x 古い正面ベクトル)
    //        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 正面ベクトル：(更新された右ベクトル x 更新された上ベクトル)
    //        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
    //        // 移動入力があった場合
    //        if (moveVec.IsNonZero())
    //        {
    //            // 移動方向を向くような、移動方向に合わせた姿勢にするために右向きベクトルを再計算
    //            Vector3 upFromAxis = playerAxes.up; // 上ベクトル：(更新された上ベクトルを取得）
    //            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 右ベクトル：(更新された上ベクトル x 移動ベクトル（移動方向 ≒ 正面ベクトル))
    //            SetPlayerAxes({ moveVec.Normalize(),rightFromMoveVec.Normalize(), playerAxes.up });
    //        }
    //    }
    //
    //    RadialBlur* radialPtr = static_cast<RadialBlur*>(PostEffectManager::GetInstance()->GetPostEffectPtr());
    //    radialPtr->SetBlurValue((std::max)(0.1f, radialPtr->GetBlurValue() - 0.02f));
    //
    //#ifdef _DEBUG
    //    GUI::Begin("player");
    //    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    //    GUI::End();
    //#endif // _DEBUG
}

void PlayerBehavior_JumpLong::RequirementCheck(void)
{
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

void IPlayerBehavior::Process_Transform(const Vector3& velocity)
{
    // どれだけ移動したのかを記録
    commonInfo_->velocity_ = velocity;

    // 新規座標
    const Vector3 position = commonInfo_->transform_.position + velocity;
    // 座標を更新
    commonInfo_->transform_.position = position;
}
