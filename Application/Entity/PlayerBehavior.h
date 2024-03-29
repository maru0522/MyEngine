#pragma once
#include <memory>
#include "Vector3.h"
#include "CameraManager.h"
#include "Timer.h"
#include "Planet.h"
#include "PlayerCommonInfomation.h"

// 前方宣言
class Player;

// 振舞いを羅列するenumクラス
enum class PlayerBehavior
{
    NONE = -1,  // 指定なし

    IDLE,       //
    STOOP,      // しゃがみ

    MOVE,       // 移動
    MOVE_STOOP, // しゃがみ移動

    JUMP,       // ジャンプ
    JUMP_STOOP, // じゃがみジャンプ
    JUMP_REVERSE,  // 反転ジャンプ
    JUMP_LONG,  // 幅跳び
};

// 振舞いを示すクラスのインターフェース
class IPlayerBehavior
{
public:
    //>> 関数
    IPlayerBehavior(Player* arg_playerPtr);
    virtual ~IPlayerBehavior(void) = default;

    // 状態遷移時の初期化処理
    virtual void Entry(void) = 0;
    // 当該状態時の様子
    virtual void Execute(void) = 0;
    // 状態遷移遷移前の終了処理
    virtual void Exit(void) = 0;

    // 他状態への遷移要件確認
    virtual void RequirementCheck(void) = 0;

    // プレイヤーのメンバ変数として生成された状態では、まだプレイヤー側でもsharedがnullptr。つまり、コンストラクタのコピーが無意味。
    // よって、初期生成時のみプレイヤーのコンストラクタでこの関数呼び出し、もう一度コピーさせることで正常化を図る。効率的なやり方知りたい。
    void CopySharedCommonInfo(const std::shared_ptr<Player_CommonInfomation>& arg_shared);

    float debug_aaaaa_;
    float debug_bbbbb_;
    float debug_ccccc_;
    float debug_ddddd_;

    Vector3 debug_eeeee_;
    Vector3 debug_fffff_;
    Vector3 debug_ggggg_;


protected:
    // 正規化済みの入力ベクトル（2次元）を返す
    const Vector2 Process_GetInput(void);
    // 重力処理を行う
    void Process_Gravity(void);
    // 座標更新処理を行う。
    void Process_Transform(const Vector3& arg_velocity);
    // モデル用のaxes計算を行う。
    void Process_CalculateModelAxes(const Vector2& arg_input);

    //>> 変数
    PlayerBehavior nextState_;
    PlayerBehavior debug_curState_;
    bool is_resetCameraPos_;

    std::shared_ptr<Player_CommonInfomation> commonInfo_;

private:
    Player* playerPtr_;

public:
    //>> getter
    PlayerBehavior GetNextState(void) { return nextState_; }
    PlayerBehavior GetCurState(void) { return debug_curState_; }
};


//----------------------------------------------------------------------------------------
// IDLE状態を示すクラス
class PlayerBehavior_Idle final : public IPlayerBehavior
{
public:
    //>> 関数
    PlayerBehavior_Idle(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_Idle(void) override = default;

    void Entry(void) override {};
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};

// STOOP状態を示すクラス
class PlayerBehavior_Stoop final : public IPlayerBehavior
{
public:
    //>> 関数
    PlayerBehavior_Stoop(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_Stoop(void) override = default;

    void Entry(void) override {};
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};

// MOVE状態を示すクラス
class PlayerBehavior_Move final : public IPlayerBehavior
{
public:
    //>> 関数
    PlayerBehavior_Move(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_Move(void) override = default;

    void Entry(void) override {};
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};

// MOVE_STOOP状態を示すクラス
class PlayerBehavior_MoveStoop final : public IPlayerBehavior
{
public:
    //>> 関数
    PlayerBehavior_MoveStoop(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_MoveStoop(void) override = default;

    void Entry(void) override {};
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};

// JUMP状態を示すクラス
class PlayerBehavior_Jump final : public IPlayerBehavior
{
private:
    //>> 定義
    const float kMove_divide_{ 1.5f }; // 空中で移動速度を元の何分の1にするか

public:
    //>> 関数
    PlayerBehavior_Jump(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_Jump(void) override = default;

    void Entry(void) override;
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;
};

// JUMP_LONG状態を示すクラス
class PlayerBehavior_JumpLong final : public IPlayerBehavior
{
public:
    //>> 関数
    PlayerBehavior_JumpLong(Player* arg_playerPtr) : IPlayerBehavior(arg_playerPtr) {}
    virtual ~PlayerBehavior_JumpLong(void) override = default;

    void Entry(void) override;
    void Execute(void) override;
    void Exit(void) override {};

private:
    void RequirementCheck(void) override;

    // 変数
    Vector2 vec2_entryInput_;
};

//----------------------------------------------------------------------------------------
// 振舞いを示すクラスを生み出すクラス
class PlayerBehaviorFactory final
{
public:
    //>> 関数
    PlayerBehaviorFactory(void) = default;
    ~PlayerBehaviorFactory(void) = default;

    std::unique_ptr<IPlayerBehavior> Create(Player* arg_playerPtr, PlayerBehavior arg_state);
};

// 振舞いを管理するクラス
class PlayerBehaviorMachine final
{
public:
    //>> 関数
    PlayerBehaviorMachine(Player* arg_playerPtr, PlayerBehavior arg_state);
    ~PlayerBehaviorMachine(void) = default;

    void ManagementBehavior(void);

private:
    // 状態遷移条件を満たしているか確認し、遷移する
    void NextStateCheck(void);

    //>> 変数
    std::unique_ptr<IPlayerBehavior> statePtr_;
    PlayerBehaviorFactory stateFactory_;
    Player* playerPtr_;

public:
    //>> getter
    IPlayerBehavior* GetStatePtr(void) { return statePtr_.get(); }
};
