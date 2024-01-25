#pragma once
#include <memory>
#include "SnakeCommonInfomation.h"

/**
 * @file SnakeBehavior.h
 * @brief 蛇の挙動全般を管理
 */

// 前方宣言
class Snake;

enum class SnakeBehavior
{
    NONE = -1, // 指定なし

    IDLE,   // idle
    MOVE,   // 通常移動（ランダムに歩き回る）
    SNEAK,  // こっそり卵に近づく。 or プレイヤーから遠ざかる等
    ESCAPE, // プレイヤーから逃げる
    DIGEST, // 襲った卵を消化中。
};

class ISnakeBehavior
{
public:
    //>> 関数
    ISnakeBehavior(void) = default;
    virtual ~ISnakeBehavior(void) = default;

    //
    virtual void Initialize(Snake* arg_snakePtr);

    // 状態遷移時の初期化処理
    virtual void Entry(void) = 0;
    // 当該状態時の様子
    virtual void Execute(void) = 0;
    // 状態遷移遷移前の終了処理
    virtual void Exit(void) = 0;

    // 他状態への遷移要件確認
    virtual void RequirementCheck(void) = 0;

protected:
    // 入力されたラジアンだけ、プレイヤーの向きを回転させる（yaw角）
    void RotateDirection(float arg_radian);

    // 縦方向の移動量に、重力分を加算する。
    void Process_Gravity(void);
    // 縦歩行の移動量と、横方向の移動量の合計を返す。
    Vector3 Process_CalculateVelocity(const Vector3& arg_velocity_horizontal = {0.f,0.f,0.f});
    // 姿勢の更新 + 一部再計算
    void Process_UpdatePosture(void);
    // 姿勢の再計算
    void Process_ReCulculatePosture(void);

    //>> 変数
    SnakeBehavior nextBehavior_{};
    SnakeBehavior currentBehavior_{};
    std::shared_ptr<SnakeCommonInfomation> commonInfo_;

public:
    //>> getter
    SnakeBehavior GetNextBehavior(void) { return nextBehavior_; }
    SnakeBehavior GetCurrentBehavior(void) { return currentBehavior_; }
};

class SnakeBehavior_Idle final : public ISnakeBehavior
{
public:
    //>> 関数
    SnakeBehavior_Idle(void) : ISnakeBehavior() {};
    virtual ~SnakeBehavior_Idle(void) override = default;

    // 初期化
    virtual void Initialize(Snake* arg_snakePtr);

    // 状態遷移時の初期化処理
    virtual void Entry(void);
    // 当該状態時の様子
    virtual void Execute(void);
    // 状態遷移遷移前の終了処理
    virtual void Exit(void) {};

private:
    // 他状態への遷移要件確認
    virtual void RequirementCheck(void);

    //>> 変数
    bool is_active_{};
    DeltaTimer timer_rest_;
};

class SnakeBehavior_Move final : public ISnakeBehavior
{
public:
    //>> 関数
    SnakeBehavior_Move(void) : ISnakeBehavior() {};
    virtual ~SnakeBehavior_Move(void) override = default;

    //　初期化
    virtual void Initialize(Snake* arg_snakePtr);

    // 状態遷移時の初期化処理
    virtual void Entry(void);
    // 当該状態時の様子
    virtual void Execute(void);
    // 状態遷移遷移前の終了処理
    virtual void Exit(void) {};

private:
    // 移動する
    void Move(void);
    // ランダムな向きを向きながら歩き回る
    void RamdomWalk(void);

    // 他状態への遷移要件確認
    virtual void RequirementCheck(void);

    //>> 変数
    bool is_needRest_{};
    float rotateDegree_{};
    DeltaTimer timer_randomWalk_;
};

class SnakeBehavior_Sneak final : public ISnakeBehavior
{
public:
    //>> 関数
    SnakeBehavior_Sneak(void) : ISnakeBehavior() {};
    virtual ~SnakeBehavior_Sneak(void) override = default;

    //　初期化
    virtual void Initialize(Snake* arg_snakePtr);

    // 状態遷移時の初期化処理
    virtual void Entry(void);
    // 当該状態時の様子
    virtual void Execute(void);
    // 状態遷移遷移前の終了処理
    virtual void Exit(void) {};

private:
    // 移動する
    void Move(void);
    // 卵に近づく
    void ApproachEgg(void);

    // 他状態への遷移要件確認
    virtual void RequirementCheck(void);

    //>> 変数
    Vector3 vec3_entryForward_;        // 遷移時の正面ベクトル
    DeltaTimer timer_rotateDirection_; // 卵の方を向くのにかける時間
};


class SnakeBehaviorFactory final
{
public:
    //>> 関数
    SnakeBehaviorFactory(void) = default;
    ~SnakeBehaviorFactory(void) = default;

    std::unique_ptr<ISnakeBehavior> Create(SnakeBehavior arg_behavior, Snake* arg_snakePtr);
};

// 振る舞いを管理するクラス
class SnakeBehaviorMachine final
{
public:
    //>> 関数
    SnakeBehaviorMachine(void) = default;
    ~SnakeBehaviorMachine(void) = default;

    void Initialize(Snake* arg_snakePtr, SnakeBehavior arg_behavior);
    void ManagementBehavior(void);

private:
    // 状態遷移条件を満たしているか確認し、遷移する
    void NextStateCheck(void);

    //>> 変数
    std::unique_ptr<ISnakeBehavior> behaviorPtr_;
    SnakeBehaviorFactory behaviorFactory_;
    Snake* snakePtr_{};

public:
    //>> getter
    ISnakeBehavior* GetStatePtr(void) { return behaviorPtr_.get(); }
};

