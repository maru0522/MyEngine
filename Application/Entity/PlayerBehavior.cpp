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
    // null繝√ぉ繝・け
    if (!statePtr_) { return; }

    // 迥ｶ諷矩・遘ｻ譚｡莉ｶ繧呈ｺ縺溘＠縺ｦ縺・ｋ縺狗｢ｺ隱阪＠縲・・遘ｻ縺吶ｋ
    NextStateCheck();

    // 迥ｶ諷区峩譁ｰ
    statePtr_->Execute();
}

void PlayerBehaviorMachine::NextStateCheck(void)
{
    // 迥ｶ諷矩・遘ｻ縺吶ｋ縺九←縺・°遒ｺ隱阪☆繧・
    statePtr_->RequirementCheck(); // 迥ｶ諷矩・遘ｻ縺吶ｋ蝣ｴ蜷医・ nextState繧貞､画峩縺吶ｋ

    // nextState縺・NONE"莉･螟悶〒縺ゅｋ蝣ｴ蜷医∫憾諷矩・遘ｻ繧定｡後≧
    PlayerBehavior nextState = statePtr_->GetNextState();
    if (nextState != PlayerBehavior::NONE)
    {
        // 譌｢蟄倥・state縺後≠繧後・邨ゆｺ・・逅・
        statePtr_->Exit();
        // 荳頑嶌縺・
        statePtr_.reset();
        statePtr_ = stateFactory_.Create(playerPtr_, nextState);
        // 蛻晄悄蛹門・逅・
        statePtr_->Entry();

        // 縺昴ｌ縺槭ｌ縺ｮ縲・謖ｯ闊槭＞"縺ｫ蠢・ｦ√↑諠・ｱ繧偵≠縺ｦ縺後≧
    }
}

//----------------------------------------------------------------------------------------
//IDLE,       //
//STOOP,      // 縺励ｃ縺後∩
//
//MOVE,       // 遘ｻ蜍・
//MOVE_STOOP, // 縺励ｃ縺後∩遘ｻ蜍・
//
//JUMP,       // 繧ｸ繝｣繝ｳ繝・
//JUMP_STOOP, // 縺倥ｃ縺後∩繧ｸ繝｣繝ｳ繝・
//JUMP_REVERSE,  // 蜿崎ｻ｢繧ｸ繝｣繝ｳ繝・
//JUMP_LONG,  // 蟷・ｷｳ縺ｳ

void PlayerBehavior_Idle::Execute(void) // "IDLE"
{
    debug_curState_ = PlayerBehavior::IDLE;

    // 驥榊鴨
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 遘ｻ蜍暮㍼ = 荳頑婿蜷・* 繧ｸ繝｣繝ｳ繝鈴㍼
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle迥ｶ諷九・驥榊鴨莉･螟悶・遘ｻ蜍暮㍼縺ｯ逋ｺ逕溘＠縺ｪ縺・Φ螳・

    // 蠎ｧ讓呎峩譁ｰ
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);

    // 蟋ｿ蜍｢蛻ｶ蠕｡
    {
        //// 迴ｾ蝨ｨ縺ｮ繝励Ξ繧､繝､繝ｼ縺ｮ蜷・ｻｸ諠・ｱ
        //const Axis3& playerAxes = GetPlayerAxes();

        //// 逅・擇縺ｮ縺ｩ縺ｮ菴咲ｽｮ縺ｫ縺・ｋ縺九↓蠢懊§縺ｦ縲∵ｭ｣縺励＞蟋ｿ蜍｢縺ｫ縺吶ｋ縺溘ａ縺ｫ3霆ｸ繧貞・險育ｮ・
        //Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // 蜿ｳ繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν x 蜿､縺・ｭ｣髱｢繝吶け繝医Ν)
        //Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // 豁｣髱｢繝吶け繝医Ν・・譖ｴ譁ｰ縺輔ｌ縺溷承繝吶け繝医Ν x 譖ｴ譁ｰ縺輔ｌ縺滉ｸ翫・繧ｯ繝医Ν)
        //SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
    }
}

void PlayerBehavior_Idle::RequirementCheck(void)
{
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    bool isLanding = GetPlayerJumpVecNorm() == 0.f;

    // 蟾ｦ繧ｷ繝輔ヨ縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_LSHIFT)
    {
        // PlayerState 繧・STOOP(縺励ｃ縺後∩)縺ｸ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }

    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_anyWASD)
    {
        // PlayerState 繧・MOVE(遘ｻ蜍・縺ｸ
        nextState_ = PlayerBehavior::MOVE;
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

void PlayerBehavior_Stoop::Execute(void) // "STOOP"
{
    debug_curState_ = PlayerBehavior::STOOP;

    // 驥榊鴨
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 遘ｻ蜍暮㍼ = 荳頑婿蜷・* 繧ｸ繝｣繝ｳ繝鈴㍼
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle迥ｶ諷九・驥榊鴨莉･螟悶・遘ｻ蜍暮㍼縺ｯ逋ｺ逕溘＠縺ｪ縺・Φ螳・

    // 蠎ｧ讓呎峩譁ｰ
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

    // 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・&& 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・↑縺・&& SPACE縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_LSHIFT == false && isDown_anyWASD == false && isDown_SPACE == false)
    {
        // PlayerState 繧・Idle縺ｸ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・ｋ && 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_anyWASD) // 縺薙％繧帝壹▲縺ｦ縺・ｋ譎らせ縺ｧ縲∝ｮ溯ｳｪ蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・ｋ
    {
        // PlayerState 繧・MOVE_STOOP(縺励ｃ縺後∩遘ｻ蜍・縺ｸ
        nextState_ = PlayerBehavior::MOVE_STOOP;
        return;
    }

    // SPACE縺悟・蜉帙＆繧後※縺・ｋ && 蝨ｰ髱｢縺ｫ雜ｳ縺後▽縺・※縺・ｋ
    if (isDown_SPACE && isLanding)
    {
        // PlayerState 繧・JUMP_STOOP(縺励ｃ縺後∩繧ｸ繝｣繝ｳ繝・縺ｸ
        nextState_ = PlayerBehavior::JUMP_STOOP;
        return;
    }
}

void PlayerBehavior_Move::Execute(void) // "MOVE"
{
    debug_curState_ = PlayerBehavior::MOVE;

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
    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveSpeed()) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

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

    // 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・&& 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・↑縺・&& SPACE縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_LSHIFT == false && isDown_anyWASD == false  && isDown_SPACE == false)
    {
        // PlayerState 繧・Idle縺ｸ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・
    if (isDown_LSHIFT == false)
    {
        // PlayerState 繧・MOVE(遘ｻ蜍・縺ｸ
        nextState_ = PlayerBehavior::MOVE;
        return;
    }

    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・↑縺・
    if (isDown_anyWASD == false)
    {
        // PlayerState 繧・STOOP(縺励ｃ縺後∩)縺ｸ
        nextState_ = PlayerBehavior::STOOP;
        return;
    }



    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ
    if (isDown_SPACE) // 縺薙％繧帝壹▲縺ｦ縺・ｋ譎らせ縺ｧ縲∝ｮ溯ｳｪ蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・ｋ
    {
        // PlayerState 繧・JUMP_LONG(蟷・ｷｳ縺ｳ)縺ｸ
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

    // 驥榊鴨
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // 遘ｻ蜍暮㍼ = 荳頑婿蜷・* 繧ｸ繝｣繝ｳ繝鈴㍼
    Vector3 velocity = GetPlayerAxes().up * GetPlayerJumpVecNorm(); // Idle迥ｶ諷九・驥榊鴨莉･螟悶・遘ｻ蜍暮㍼縺ｯ逋ｺ逕溘＠縺ｪ縺・Φ螳・

    // 蠎ｧ讓呎峩譁ｰ
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
}

void PlayerBehavior_Jump::RequirementCheck(void)
{
    bool isDown_anyWASD = (bool)(KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S));
    bool isDown_LSHIFT = KEYS::IsDown(DIK_LSHIFT);
    //bool isDown_SPACE = KEYS::IsDown(DIK_SPACE);
    //bool isLanding = GetPlayerJumpVecNorm() == 0.f;

    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・↑縺・&& 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・
    if (isDown_anyWASD == false && isDown_LSHIFT == false)
    {
        // PlayerState 繧・IDLE縺ｸ
        nextState_ = PlayerBehavior::IDLE;
        return;
    }

    // 遘ｻ蜍輔く繝ｼ縺悟・蜉帙＆繧後※縺・ｋ && 蟾ｦSHIFT縺悟・蜉帙＆繧後※縺・↑縺・
    if (KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S) != 0 &&
        KEYS::IsDown(DIK_LSHIFT) == false)
    {
        nextState_ = PlayerBehavior::MOVE;
        return;
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
    SetPlayerJumpVecNorm(GetPlayerJumpPower() / 3);
}

void PlayerBehavior_JumpLong::Execute(void)
{
    debug_curState_ = PlayerBehavior::JUMP_LONG;


    // 蟷・ｷｳ縺ｳ縺吶ｋ譁ｹ蜷代・繧ｯ繝医Ν
    Vector3 vec3_jumpLong = GetPlayerMoveVec();

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
    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveSpeed()) + (GetPlayerAxes().up * GetPlayerJumpVecNorm());

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
