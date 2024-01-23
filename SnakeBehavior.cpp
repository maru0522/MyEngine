#include "SnakeBehavior.h"
#include "Snake.h"

void ISnakeBehavior::Initialize(Snake* arg_snakePtr)
{
    // 基底クラスなので、behaviorはnone
    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::NONE;

    // 汎用情報をコピーして共有する。
    commonInfo_ = arg_snakePtr->commonInfo_;
}

void ISnakeBehavior::Process_Gravity(void)
{
    // 重力
    commonInfo_->velocity_vertical_ -= SnakeCommonInfomation::kGravity_;
}

Vector3 ISnakeBehavior::Process_CalculateVelocity(const Vector3& arg_velocity_horizontal)
{
    // 垂直方向の移動量
    Vector3 velocity_vertical = commonInfo_->axes_.up * commonInfo_->velocity_vertical_;
    // 合計の移動量 = 垂直方向の移動量 + 平行方向の移動量
    return velocity_vertical + arg_velocity_horizontal;
}

void ISnakeBehavior::Process_UpdatePosture(void)
{
    // 姿勢を正常に保つため 
    commonInfo_->axes_.up = commonInfo_->vec3_newUp_;                                               // 新規上ベクトルの設定
    commonInfo_->axes_.forward = commonInfo_->vec3_moveDirection_;                                  // 移動する方向を正面ベクトルとして設定
    // あとで、正面ベクトルを再定義するために、右vecも更新しておく。
    commonInfo_->axes_.right = Math::Vec3::Cross(commonInfo_->axes_.up, commonInfo_->axes_.forward).Normalize(); // 右ベクトルを計算し、設定（新規の上vec x 正面vec）
}

void ISnakeBehavior::Process_ReCulculatePosture(void)
{
    // 現在の上ベクトルと右ベクトルから正面ベクトルを再定義
    // 正面ベクトルが、プレイヤーを検知した瞬間のままだと、移動するにつれ兎がつぶれる（姿勢が正常ではないため）
    const Vector3 forward = Math::Vec3::Cross(commonInfo_->axes_.right, commonInfo_->axes_.up);
    // 再定義したベクトルを正面ベクトルに設定
    commonInfo_->axes_.forward = forward;
    // 再定義したベクトルを移動方向ベクトルに設定。
    commonInfo_->vec3_moveDirection_ = forward;
}
// --------------------------------------------------------------------------------------


void SnakeBehavior_Idle::Initialize(Snake* arg_snakePtr)
{
    ISnakeBehavior::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::IDLE;
}

void SnakeBehavior_Idle::Execute(void)
{
    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 重力処理
    Process_Gravity();
    // 移動総量計算
    const Vector3& velocity_total = Process_CalculateVelocity();
    // 座標更新
    commonInfo_->transform_.position += velocity_total;

    // 姿勢再計算
    Process_ReCulculatePosture();
}


// -------------------------------------------------------------------------------------
std::unique_ptr<ISnakeBehavior> SnakeBehaviorFactory::Create(SnakeBehavior arg_behavior, Snake* arg_snakePtr)
{
    // 生成されたbehaviorを収納する変数
    std::unique_ptr<ISnakeBehavior> behavior;

    // 生成
    if (arg_behavior == SnakeBehavior::IDLE) { behavior = std::make_unique<SnakeBehavior_Idle>(); }

    // 初期化関数の実行
    behavior->Initialize(arg_snakePtr);

    // リターン
    return std::move(behavior);
}


// -------------------------------------------------------------------------------------
void SnakeBehaviorMachine::Initialize(Snake* arg_snakePtr, SnakeBehavior arg_behavior)
{
    // 一番最初の振る舞いを生成。
    behaviorPtr_ = behaviorFactory_.Create(arg_behavior, arg_snakePtr);
}

void SnakeBehaviorMachine::ManagementBehavior(void)
{
    // nullチェック
    if (!behaviorPtr_) { return; }

    // 状態遷移条件を満たしているか確認し、遷移する
    NextStateCheck();

    // 状態更新
    behaviorPtr_->Execute();
}

void SnakeBehaviorMachine::NextStateCheck(void)
{
    // 状態遷移するかどうか確認する
    behaviorPtr_->RequirementCheck(); // 状態遷移する場合は nextStateを変更する

    // nextStateが"NONE"以外である場合、状態遷移を行う
    SnakeBehavior nextState = behaviorPtr_->GetNextBehavior();
    if (nextState != SnakeBehavior::NONE)
    {
        // 既存のstateがあれば終了処理
        behaviorPtr_->Exit();
        // 上書き
        behaviorPtr_.reset();
        behaviorPtr_ = behaviorFactory_.Create(nextState, snakePtr_);
        // 初期化処理
        behaviorPtr_->Entry();

        // それぞれの、"振舞い"に必要な情報をあてがう
    }
}
