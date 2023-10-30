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

    // カメラ制御
    {
        // プレイヤーの座標
        const Vector3& player_pos = GetPlayerTransform().position;
        // プレイヤー追従カメラの座標
        const Vector3& camera_pos = GetPlayerCamMPtr()->GetCurrentCamera()->GetTransformPtr()->position;


        // プレイヤーの正面ベクトル
        const Vector3& vec_player_forward = GetPlayerAxes().forward.Normalize();
        // カメラからプレイヤー方向へのベクトル
        Vector3 vec_cameraToPlayer = (player_pos - camera_pos).Normalize();
        // プレイヤーの正面ベクトルと、カメラからプレイヤーへの方向ベクトルの内積値
        float dot_pf_c2p = Math::Vec3::Dot(vec_player_forward, vec_cameraToPlayer);
        
        // 内積値が0.6fより大きい
        if (dot_pf_c2p > 0.6f)
        {
            // カメラ位置リセットを true
            is_resetCameraPos_ = true;
        }

        // カメラの位置を初期化
        if (is_resetCameraPos_)
        {

        }
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
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle迥ｶ諷九・驥榊鴨莉･螟悶・遘ｻ蜍暮㍼縺ｯ逋ｺ逕溘＠縺ｪ縺・Φ螳・

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

        Vector3 vec_cam2p = (GetPlayerTransform().position - GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos()).Normalize();

        // プレイヤーの正面ベクトルと、カメラからプレイヤーへのベクトルの内積が -0.6f未満
        if (GetPlayerAxes().forward.Dot(vec_cam2p) < -0.6f)
        {

        }

        //// プレイヤーの正面とカメラの正面の内積が "規定値" 未満の時
        //// 規定値の値を小さくするほど、プレイヤーが画面中央に近い位置で、カメラの挙動が切り替わる。
        //if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
        //{
        //    // カメラとプレイヤーの距離
        //    float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

        //    // 該当距離が、本来設定されているプレイヤーとの距離より短い場合、該当距離を設定距離とする。
        //    if (dist < GetPlayerCurrentRad())
        //    {
        //        // プレイヤーがカメラ側に向かって移動する際、カメラの座標を固定する意図
        //        // しかし、現状だとカメラが遠ざかる処理が上手く機能していない為コメントアウト。
        //        //current_rad_ = dist;
        //    }
        //}
        //else
        //{
        //    // 現在距離(cureent_rad_)が、初期距離(default_rad_)より小さい値なら、現在距離を補正する。
        //    if (GetPlayerCurrentRad() < GetPlayerDefaultRad())
        //    {
        //        SetPlayerCurrentRad(GetPlayerCurrentRad() + 0.1f);
        //        //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
        //    }
        //    else if (GetPlayerCurrentRad() > GetPlayerDefaultRad())
        //    {
        //        SetPlayerCurrentRad(GetPlayerCurrentRad() - 0.1f);
        //    }

        //    float theta = GetPlayerTheta();
        //    float phi = GetPlayerPhi();

        //    theta += 0.02f * inputVec.y;
        //    phi += 0.02f * inputVec.x;
        //    Math::Function::Loop(theta, 0.f, 6.28319f);
        //    Math::Function::Loop(phi, 0.f, 6.28319f);

        //    SetPlayerTheta(theta);
        //    SetPlayerPhi(phi);
        //}


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

    // 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・&& 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・↑縺・&& SPACE縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_LSHIFT == false && isDown_anyWASD == false && isDown_SPACE == false)
    {
        // PlayerState 繧・IDLE縺ｸ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 蟾ｦ繧ｷ繝輔ヨ縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_LSHIFT) // 縺薙％繧帝壹▲縺ｦ縺・ｋ譎らせ縺ｧ縲∝ｮ溯ｳｪ遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ
    {
        // PlayerState 繧・MOVE_STOOP(縺励ｃ縺後∩遘ｻ蜍・縺ｸ
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
    }

    // SPACE縺悟・蜉帙＆繧後※縺・ｋ && 蝨ｰ髱｢縺ｫ雜ｳ縺後▽縺・※縺・ｋ
    if (isDown_SPACE && isLanding)
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
    inputVec = inputVec.Normalize();

    // 繧ｫ繝｡繝ｩ隕也せ縺ｮ繝励Ξ繧､繝､繝ｼ遘ｻ蜍輔・繧ｯ繝医Ν
    Vector3 pForwardFromCamera = Math::Vec3::Cross(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->right, GetPlayerAxes().up).Normalize(); // 豁｣髱｢Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(GetPlayerAxes().up, pForwardFromCamera).Normalize(); // 蜿ｳVec: cross(p.upVec, pForwardFromCamera)

    // 遘ｻ蜍輔・繧ｯ繝医Ν = 蜑榊ｾ計ec + 豌ｴ蟷ｳvec
    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);

    // 繧ｫ繝｡繝ｩ蠎ｧ讓咏畑縺ｮ蛟､繧定｣懈ｭ｣
    {
        if (GetPlayerJumpVecNorm())
        {
            // 繧ｫ繝｡繝ｩ縺ｨ繝励Ξ繧､繝､繝ｼ縺ｮ霍晞屬
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // 繧ｸ繝｣繝ｳ繝玲凾縺ｫ繧ｫ繝｡繝ｩ縺ｮ霑ｽ蠕薙′霆ｽ貂・竕・逕ｻ髱｢縺ｮ謠ｺ繧後ｒ謚大宛縺吶ｋ逶ｮ逧・
            // 蜀・ｩ阪′隕丞ｮ壼､譛ｪ貅縺ｮ譎ゅず繝｣繝ｳ繝励ｒ郢ｰ繧願ｿ斐☆縺ｨ繧ｫ繝｡繝ｩ霍晞屬縺ｩ繧薙←繧馴□縺上↑縺｣縺ｦ縺・￥荳榊・蜷医′蜃ｺ縺ｦ繧・
            SetPlayerCurrentRad(dist);
        }

        // 繝励Ξ繧､繝､繝ｼ縺ｮ豁｣髱｢縺ｨ繧ｫ繝｡繝ｩ縺ｮ豁｣髱｢縺ｮ蜀・ｩ阪′ "隕丞ｮ壼､" 譛ｪ貅縺ｮ譎・
        // 隕丞ｮ壼､縺ｮ蛟､繧貞ｰ上＆縺上☆繧九⊇縺ｩ縲√・繝ｬ繧､繝､繝ｼ縺檎判髱｢荳ｭ螟ｮ縺ｫ霑代＞菴咲ｽｮ縺ｧ縲√き繝｡繝ｩ縺ｮ謖吝虚縺悟・繧頑崛繧上ｋ縲・
        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
        {
            // 繧ｫ繝｡繝ｩ縺ｨ繝励Ξ繧､繝､繝ｼ縺ｮ霍晞屬
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // 隧ｲ蠖楢ｷ晞屬縺後∵悽譚･險ｭ螳壹＆繧後※縺・ｋ繝励Ξ繧､繝､繝ｼ縺ｨ縺ｮ霍晞屬繧医ｊ遏ｭ縺・ｴ蜷医∬ｩｲ蠖楢ｷ晞屬繧定ｨｭ螳夊ｷ晞屬縺ｨ縺吶ｋ縲・
            if (dist < GetPlayerCurrentRad())
            {
                // 繝励Ξ繧､繝､繝ｼ縺後き繝｡繝ｩ蛛ｴ縺ｫ蜷代°縺｣縺ｦ遘ｻ蜍輔☆繧矩圀縲√き繝｡繝ｩ縺ｮ蠎ｧ讓吶ｒ蝗ｺ螳壹☆繧区э蝗ｳ
                // 縺励°縺励∫樟迥ｶ縺縺ｨ繧ｫ繝｡繝ｩ縺碁□縺悶°繧句・逅・′荳頑焔縺乗ｩ溯・縺励※縺・↑縺・ぜ繧ｳ繝｡繝ｳ繝医い繧ｦ繝医・
                //current_rad_ = dist;
            }
        }
        else
        {
            // 迴ｾ蝨ｨ霍晞屬(cureent_rad_)縺後∝・譛溯ｷ晞屬(default_rad_)繧医ｊ蟆上＆縺・､縺ｪ繧峨∫樟蝨ｨ霍晞屬繧定｣懈ｭ｣縺吶ｋ縲・
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

    // 驥榊鴨
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 遘ｻ蜍暮㍼ = 遘ｻ蜍夫ec * 遘ｻ蜍暮溷ｺｦ + 荳頑婿蜷・* 繧ｸ繝｣繝ｳ繝鈴㍼
    Vector3 velocity = (moveVec.Normalize() * (GetPlayerMoveSpeed() / 2)) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

    // 蠎ｧ讓呎峩譁ｰ
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
    SetPlayerMoveVec(moveVec);

    // 蟋ｿ蜍｢蛻ｶ蠕｡
    {
        // 迴ｾ蝨ｨ縺ｮ繝励Ξ繧､繝､繝ｼ縺ｮ蜷・ｻｸ諠・ｱ
        const Axis3& playerAxes = GetPlayerAxes();

        // 逅・擇縺ｮ縺ｩ縺ｮ菴咲ｽｮ縺ｫ縺・ｋ縺九↓蠢懊§縺ｦ縲∵ｭ｣縺励＞蟋ｿ蜍｢縺ｫ縺吶ｋ縺溘ａ縺ｫ3霆ｸ繧貞・險育ｮ・
        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 蜿ｳ繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν x 蜿､縺・ｭ｣髱｢繝吶け繝医Ν)
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 豁｣髱｢繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺溷承繝吶け繝医Ν x 譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν)
        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
        // 遘ｻ蜍募・蜉帙′縺ゅ▲縺溷ｴ蜷・
        if (moveVec.IsNonZero())
        {
            // 遘ｻ蜍墓婿蜷代ｒ蜷代￥繧医≧縺ｪ縲∫ｧｻ蜍墓婿蜷代↓蜷医ｏ縺帙◆蟋ｿ蜍｢縺ｫ縺吶ｋ縺溘ａ縺ｫ蜿ｳ蜷代″繝吶け繝医Ν繧貞・險育ｮ・
            Vector3 upFromAxis = playerAxes.up; // 荳翫・繧ｯ繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν繧貞叙蠕暦ｼ・
            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // 蜿ｳ繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν x 遘ｻ蜍輔・繧ｯ繝医Ν・育ｧｻ蜍墓婿蜷・竕・豁｣髱｢繝吶け繝医Ν))
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
    bool isTrigger_SPACE = KEYS::IsTrigger(DIK_SPACE);
    bool isLanding = GetPlayerIsLanding();

    // 左SHIFTが入力されていない && 移動キーが入力されていない && SPACEが入力されている
    if (isDown_LSHIFT == false && isDown_anyWASD == false && isDown_SPACE == false)
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



    // 着地している && 移動キーが入力された
    if (isLanding && isTrigger_SPACE) // ここを通っている時点で、実質左SHIFTが入力されている
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

    float moveSpeedDived = kMove_divide_;
    if (moveSpeedDived == 0.f) moveSpeedDived = 1.f; // 0除算チェック

    // 移動量 = 移動vec * (移動速度 / 規定割合) + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * (GetPlayerMoveSpeed() / moveSpeedDived)) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

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

void PlayerBehavior_Jump::RequirementCheck(void)
{
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    bool isLanding = GetPlayerIsLanding();

    // 着地している
    if (isLanding)
    {
        // 移動キーを押していない && 左SHIFTを押していない
        if (isDown_anyWASD == false && isDown_LSHIFT == false)
        {
            // PlayerState をIdleへ
            nextState_ = PlayerBehavior::IDLE;
            return;
        }

        // 移動キーを押している && 左SHIFTを押していない
        if (isDown_anyWASD && isDown_LSHIFT == false)
        {
            nextState_ = PlayerBehavior::MOVE;
            return;
        }
    }

    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ && 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・ｋ
    if (KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S) != 0 &&
        KEYS::IsDown(DIK_LSHIFT))
    {
        nextState_ = PlayerBehavior::STOOP;
        return;
    }
}

void PlayerBehavior_JumpLong::Entry(void)
{
    SetPlayerJumpVecNorm(GetPlayerJumpLongPower());
}

void PlayerBehavior_JumpLong::Execute(void)
{
    debug_curState_ = PlayerBehavior::JUMP_LONG;

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
        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.4f)
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

            theta += 0.1f * moveVec.y;
            phi += 0.1f * moveVec.x;
            Math::Function::Loop(theta, 0.f, 6.28319f);
            Math::Function::Loop(phi, 0.f, 6.28319f);

            SetPlayerTheta(theta);
            SetPlayerPhi(phi);
        }
    }

    // 重力
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    //// 移動量 = 移動vec * (移動速度 / 規定割合) + 上方向 * ジャンプ量
    //Vector3 velocity = ((habatobiVec.Normalize() * (GetPlayerMoveSpeed() + 5)) - (inputMoveVec.Normalize() * GetPlayerMoveSpeed())) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

    // 移動量 = 移動vec * (移動速度 + 5) + 上方向 * ジャンプ量
    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveJumpLongSpeed()) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());


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
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isLanding = GetPlayerIsLanding();

    // 着地している
    if (isLanding)
    {
        // 移動キーを押していない
        if (isDown_anyWASD == false)
        {
            // PlayerState をIdleへ
            nextState_ = PlayerBehavior::IDLE;
            return;
        }

        // 移動キーを押している
        if (isDown_anyWASD)
        {
            nextState_ = PlayerBehavior::MOVE;
            return;
        }
    }
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

float IPlayerBehavior::GetPlayerMoveJumpLongSpeed(void)
{
    return playerPtr_->kMoveJumpLongSpeed_;
}

bool IPlayerBehavior::GetPlayerIsLanding(void)
{
    return playerPtr_->isLanding_;
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
