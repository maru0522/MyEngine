#pragma once
#include <memory>
#include "Vector3.h"
#include "CameraManager.h"

// �O���錾
class Player; // ���

// �U�����𗅗񂷂�enum�N���X
enum class PlayerBehavior
{
    NONE = -1,  // �w��Ȃ�

    IDLE,       //

    MOVE,       // �ړ�
    //WALK,       // ����
    //RUN,        // ����

    STOOP,      // ���Ⴊ��

    JUMP,       // �W�����v
    JUMP_STOOP, // ���Ⴊ�݃W�����v
    JUMP_BACK,  // ���]�W�����v
    JUMP_LONG,  // ������
};

// �U�����������N���X�̃C���^�[�t�F�[�X
class IPlayerBehavior
{
public:
    //>> �֐�
    IPlayerBehavior(Player* arg_playerPtr) : 
        nextState_(PlayerBehavior::NONE), playerPtr_(arg_playerPtr) {};
    virtual ~IPlayerBehavior(void) = default;

    // ��ԑJ�ڎ��̏���������
    virtual void Entry(void) = 0;
    // ���Y��Ԏ��̗l�q
    virtual void Execute(void) = 0;
    // ��ԑJ�ڑJ�ڑO�̏I������
    virtual void Exit(void) = 0;

    // ����Ԃւ̑J�ڗv���m�F
    virtual void RequirementCheck(void) = 0;

protected:
    //>> �ϐ�
    PlayerBehavior nextState_;
    PlayerBehavior debug_curState_;

private:
    Player* playerPtr_;

public:
    //>> getter
    PlayerBehavior GetNextState(void) { return nextState_; }
    PlayerBehavior GetCurState(void) { return debug_curState_; }

protected:
    // �z�Q�Ɖ���̂��߁Ah���ɒ�`�o����
    const Vector3& GetPlayerMoveVec(void);
    const Vector3& GetPlayerVelociy(void);
    const Axis3& GetPlayerAxes(void);
    const Transform& GetPlayerTransform(void);
    float GetPlayerJumpVecNorm(void);
    float GetPlayerDefaultRad(void);
    float GetPlayerCurrentRad(void);
    float GetPlayerTheta(void);
    float GetPlayerPhi(void);
    float GetPlayerGravity(void);
    float GetPlayerJumpPower(void);
    float GetPlayerMoveSpeed(void);
    CameraManager* GetPlayerCamMPtr(void);

    //>> setter
    void SetPlayerMoveVec(const Vector3& arg_moveVec);
    void SetPlayerVelocity(const Vector3& arg_velocity);
    void SetPlayerAxes(const Axis3& arg_axes);
    void SetPlayerJumpVecNorm(float arg_jumpVecNorm);
    void SetPlayerCurrentRad(float arg_currentRad);
    void SetPlayerTheta(float arg_theta);
    void SetPlayerPhi(float arg_phi);
    void SetPlayerTransformPosition(const Vector3& arg_pos);
};


//----------------------------------------------------------------------------------------
// IDLE��Ԃ������N���X
class PlayerBehavior_Idle final : public IPlayerBehavior
{
public:
    //>> �֐�
    PlayerBehavior_Idle(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_Idle(void) override = default;

    void Entry(void) override {};
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};

// MOVE��Ԃ������N���X
class PlayerBehavior_Move final : public IPlayerBehavior
{
public:
    //>> �֐�
    PlayerBehavior_Move(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_Move(void) override = default;

    void Entry(void) override {};
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};


//----------------------------------------------------------------------------------------
// �U�����������N���X�𐶂ݏo���N���X
class PlayerBehaviorFactory final
{
public:
    //>> �֐�
    PlayerBehaviorFactory(void) = default;
    ~PlayerBehaviorFactory(void) = default;

    std::unique_ptr<IPlayerBehavior> Create(Player* arg_playerPtr, PlayerBehavior arg_state);
};

// �U�������Ǘ�����N���X
class PlayerBehaviorMachine final
{
public:
    //>> �֐�
    PlayerBehaviorMachine(Player* arg_playerPtr, PlayerBehavior arg_state);
    ~PlayerBehaviorMachine(void) = default;

    void ManagementBehavior(void);

private:
    // ��ԑJ�ڏ����𖞂����Ă��邩�m�F���A�J�ڂ���
    void NextStateCheck(void);

    //>> �ϐ�
    std::unique_ptr<IPlayerBehavior> statePtr_;
    PlayerBehaviorFactory stateFactory_;
    Player* playerPtr_;

public:
    //>> getter
    IPlayerBehavior* GetStatePtr(void) { return statePtr_.get(); }
};
