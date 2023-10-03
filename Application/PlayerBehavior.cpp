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
    if (arg_state == PlayerBehavior::MOVE) { return std::make_unique<PlayerBehavior_Move>(arg_playerPtr); }
    //if (arg_state == PlayerState::WALK) { return std::make_unique<PlayerState_Idle>(); }
    //if (arg_state == PlayerState::RUN) { return std::make_unique<PlayerState_Idle>(); }
    if (arg_state == PlayerBehavior::STOOP) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP_STOOP) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
    if (arg_state == PlayerBehavior::JUMP_BACK) { return std::make_unique<PlayerBehavior_Idle>(arg_playerPtr); }
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
    // null�`�F�b�N
    if (!statePtr_) { return; }

    // ��ԑJ�ڏ����𖞂����Ă��邩�m�F���A�J�ڂ���
    NextStateCheck();

    // ��ԍX�V
    statePtr_->Execute();
}

void PlayerBehaviorMachine::NextStateCheck(void)
{
    // ��ԑJ�ڂ��邩�ǂ����m�F����
    statePtr_->RequirementCheck(); // ��ԑJ�ڂ���ꍇ�� nextState��ύX����

    // nextState��"NONE"�ȊO�ł���ꍇ�A��ԑJ�ڂ��s��
    PlayerBehavior nextState = statePtr_->GetNextState();
    if (nextState != PlayerBehavior::NONE)
    {
        // ������state������ΏI������
        statePtr_->Exit();
        // �㏑��
        statePtr_.reset();
        statePtr_ = stateFactory_.Create(playerPtr_, nextState);
        // ����������
        statePtr_->Entry();

        // ���ꂼ��́A"�U����"�ɕK�v�ȏ������Ă���
    }
}

void PlayerBehavior_Idle::Execute(void)
{
    // �d��
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // �W�����v�x�N�g��
    Vector3 jumpVec = GetPlayerAxes().up;

    // �ړ��� = �W�����vvec * �W�����v��
    Vector3 velocity = jumpVec * GetPlayerJumpVecNorm(); // Idle��Ԃ͏d�͈ȊO�̈ړ��ʂ͔������Ȃ��z��

    // ���W�X�V
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);

    // �p������
    {
        //// ���݂̃v���C���[�̊e�����
        //const Axis3& playerAxes = GetPlayerAxes();

        //// ���ʂ̂ǂ̈ʒu�ɂ��邩�ɉ����āA�������p���ɂ��邽�߂�3�����Čv�Z
        //Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �Â����ʃx�N�g��)
        //Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // ���ʃx�N�g���F(�X�V���ꂽ�E�x�N�g�� x �X�V���ꂽ��x�N�g��)
        //SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
    }
}

//----------------------------------------------------------------------------------------
void PlayerBehavior_Idle::RequirementCheck(void) // "IDLE"
{
    // �ړ��L�[�����͂���Ă�����
    if (KEYS::IsDown(DIK_D) || KEYS::IsDown(DIK_A) || KEYS::IsDown(DIK_W) || KEYS::IsDown(DIK_S))
    {
        // PlayerState �� MOVE��
        nextState_ = PlayerBehavior::MOVE;
        return;
    }
}

void PlayerBehavior_Move::Execute(void)
{
    debug_curState_ = PlayerBehavior::MOVE;

    // ���̓x�N�g��
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec = inputVec.Normalize();

    // �J�������_�̃v���C���[�ړ��x�N�g��
    Vector3 pForwardFromCamera = Math::Vec3::Cross(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->right, GetPlayerAxes().up).Normalize(); // ����Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(GetPlayerAxes().up, pForwardFromCamera).Normalize(); // �EVec: cross(p.upVec, pForwardFromCamera)

    // �ړ��x�N�g�� = �O��vec + ����vec
    Vector3 moveVec = (pForwardFromCamera * inputVec.y) + (redefinitionPRightFromCamera * inputVec.x);

    // �J�������W�p�̒l��␳
    {
        if (GetPlayerJumpVecNorm())
        {
            // �J�����ƃv���C���[�̋���
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // �W�����v���ɃJ�����̒Ǐ]���y�� �� ��ʂ̗h���}������ړI
            // ���ς��K��l�����̎��W�����v���J��Ԃ��ƃJ���������ǂ�ǂ񉓂��Ȃ��Ă����s����o�Ă�
            SetPlayerCurrentRad(dist);
        }

        // �v���C���[�̐��ʂƃJ�����̐��ʂ̓��ς� "�K��l" �����̎�
        // �K��l�̒l������������قǁA�v���C���[����ʒ����ɋ߂��ʒu�ŁA�J�����̋������؂�ւ��B
        if (GetPlayerAxes().forward.Dot(GetPlayerCamMPtr()->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
        {
            // �J�����ƃv���C���[�̋���
            float dist = (GetPlayerCamMPtr()->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - GetPlayerTransform().position).Length();

            // �Y���������A�{���ݒ肳��Ă���v���C���[�Ƃ̋������Z���ꍇ�A�Y��������ݒ苗���Ƃ���B
            if (dist < GetPlayerCurrentRad())
            {
                // �v���C���[���J�������Ɍ������Ĉړ�����ہA�J�����̍��W���Œ肷��Ӑ}
                // �������A���󂾂ƃJ�������������鏈������肭�@�\���Ă��Ȃ��׃R�����g�A�E�g�B
                //current_rad_ = dist;
            }
        }
        else
        {
            // ���݋���(cureent_rad_)���A��������(default_rad_)��菬�����l�Ȃ�A���݋�����␳����B
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

    // �d��
    SetPlayerJumpVecNorm(GetPlayerJumpVecNorm() - GetPlayerGravity());

    // �W�����v�x�N�g��
    Vector3 jumpVec = GetPlayerAxes().up;
    if (KEYS::IsTrigger(DIK_SPACE)) { SetPlayerJumpVecNorm(GetPlayerJumpPower()); }

    // �ړ��� = �ړ�vec * �ړ����x + �W�����vvec * �W�����v��
    Vector3 velocity = (moveVec.Normalize() * GetPlayerMoveSpeed()) + (jumpVec * GetPlayerJumpVecNorm());

    // ���W�X�V
    SetPlayerTransformPosition(GetPlayerTransform().position + velocity);
    SetPlayerVelocity(velocity);
    SetPlayerMoveVec(moveVec);

    // �p������
    {
        // ���݂̃v���C���[�̊e�����
        const Axis3& playerAxes = GetPlayerAxes();

        // ���ʂ̂ǂ̈ʒu�ɂ��邩�ɉ����āA�������p���ɂ��邽�߂�3�����Čv�Z
        Vector3 rightFromOldAxis = Math::Vec3::Cross(playerAxes.up, playerAxes.forward); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �Â����ʃx�N�g��)
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(rightFromOldAxis.Normalize(), playerAxes.up); // ���ʃx�N�g���F(�X�V���ꂽ�E�x�N�g�� x �X�V���ꂽ��x�N�g��)
        SetPlayerAxes({ forwardFromOldAxis.Normalize(),rightFromOldAxis.Normalize(),playerAxes.up });
        // �ړ����͂��������ꍇ
        if (moveVec.IsNonZero())
        {
            // �ړ������������悤�ȁA�ړ������ɍ��킹���p���ɂ��邽�߂ɉE�����x�N�g�����Čv�Z
            Vector3 upFromAxis = playerAxes.up; // ��x�N�g���F(�X�V���ꂽ��x�N�g�����擾�j
            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �ړ��x�N�g���i�ړ����� �� ���ʃx�N�g��))
            SetPlayerAxes({ moveVec.Normalize(),rightFromMoveVec.Normalize(), playerAxes.up });
        }
    }

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::End();
#endif // _DEBUG
}

void PlayerBehavior_Move::RequirementCheck(void) // "MOVE"
{
    // �ړ��L�[�����͂���Ă��Ȃ�������
    if (KEYS::IsDown(DIK_D) + KEYS::IsDown(DIK_A) + KEYS::IsDown(DIK_W) + KEYS::IsDown(DIK_S) == 0)
    {
        // PlayerState �� IDLE��
        nextState_ = PlayerBehavior::IDLE;
        return;
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
