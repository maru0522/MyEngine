#include "SnakeBehavior.h"
#include "Snake.h"
#include "SimplifyImGui.h"

#ifdef _DEBUG
#include "Input.h"
#endif // _DEBUG


void ISnakeBehavior::Initialize(Snake* arg_snakePtr)
{
    // 基底クラスなので、behaviorはnone
    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::NONE;

    // 汎用情報をコピーして共有する。
    commonInfo_ = arg_snakePtr->commonInfo_;
}

void ISnakeBehavior::RotateDirection(float arg_radian)
{
    //##姿勢の上方向を軸に "arg_radian" 回転する
    // 回転用クオータニオン
    Quaternion rotQ = Math::QuaternionF::MakeAxisAngle(commonInfo_->axes_.up, arg_radian);
    // クオータニオンを使用して、正面ベクトルを回転
    commonInfo_->axes_.forward = Math::QuaternionF::RotateVector(commonInfo_->axes_.forward, rotQ);
    // クオータニオンを使用して、右ベクトルを回転
    commonInfo_->axes_.right = Math::QuaternionF::RotateVector(commonInfo_->axes_.right, rotQ);
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
    // 姿勢を正常に保つため ※代入は、各ベクトルが0ではない時に限る
    if (commonInfo_->vec3_newUp_.IsNonZero()) { commonInfo_->axes_.up = commonInfo_->vec3_newUp_; }                      // 新規上ベクトルの設定
    if (commonInfo_->vec3_moveDirection_.IsNonZero()) { commonInfo_->axes_.forward = commonInfo_->vec3_moveDirection_; } // 移動する方向を正面ベクトルとして設定
    // あとで、正面ベクトルを再定義するために、右vecも更新しておく。
    commonInfo_->axes_.right = Math::Vec3::Cross(commonInfo_->axes_.up, commonInfo_->axes_.forward).Normalize();         // 右ベクトルを計算し、設定（新規の上vec x 正面vec）
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

void SnakeBehavior_Idle::Entry(void)
{
    is_active_ = false;

    // どのくらいの時間休憩するのか、"kTimer_rest_min_" ~ "kTiImer_rest_max_"の間からランダムに決定
    const float restTime = Math::Function::Random<float>(SnakeCommonInfomation::kTimer_rest_min_, SnakeCommonInfomation::kTimer_rest_max_);
    timer_rest_.Start(restTime);
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

    // タイマの更新
    timer_rest_.Update();
    if (timer_rest_.GetTimeRate() >= 1.f) { is_active_ = true; }
}

void SnakeBehavior_Idle::RequirementCheck(void)
{
    // 休憩時間が満了した
    if (is_active_)
    {
        // 蛇の振る舞いをMOVEへ変更
        nextBehavior_ = SnakeBehavior::MOVE;
        return;
    }

    // 卵の存在を検知した。
    if (commonInfo_->is_detectEgg_)
    {
        // 蛇の振る舞いをSNEAKへ変更
        nextBehavior_ = SnakeBehavior::SNEAK;
        return;
    }
}

void SnakeBehavior_Move::Initialize(Snake* arg_snakePtr)
{
    ISnakeBehavior::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::MOVE;
}

void SnakeBehavior_Move::Entry(void)
{
    // フラグ初期化
    is_needRest_ = false;

    // 回転角度決定
    rotateDegree_ = Math::Function::Random<float>(SnakeCommonInfomation::kDegree_randomWalk_min_, SnakeCommonInfomation::kDegree_randomWalk_max_);
    // 回転方向
    const int32_t threshold = Math::Function::Random<int32_t>(-9, 9);
    const int32_t randomNum = Math::Function::Random<int32_t>(-10, 10);
    const bool is_rotateLeft = randomNum > threshold;
    // 回転方向を考慮する
    if (is_rotateLeft) { rotateDegree_ *= -1; }

    // どのくらいの時間ランダムに移動し続けるのか、"kTimer_randomWalk_min_" ~ "kTiImer_randomWalk_max_"の間からランダムに決定
    const float activeTime = Math::Function::Random<float>(SnakeCommonInfomation::kTimer_randomWalk_min_, SnakeCommonInfomation::kTimer_randomWalk_max_);
    timer_randomWalk_.Start(activeTime);
}

void SnakeBehavior_Move::Execute(void)
{
    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // ランダムに動き回る。
    RamdomWalk();

    // 姿勢再計算
    Process_ReCulculatePosture();
}

void SnakeBehavior_Move::Move(void)
{
    // 重力処理（垂直方向の移動量）
    Process_Gravity();
    // 正面への移動（平行方向への移動）
    const Vector3 velocity_horizontal = commonInfo_->axes_.forward * commonInfo_->kMoveSpd_default_;

    // 移動総量計算
    const Vector3& velocity_total = Process_CalculateVelocity(velocity_horizontal);
    // 座標更新
    commonInfo_->transform_.position += velocity_total;
}

void SnakeBehavior_Move::RamdomWalk(void)
{
#ifdef _DEBUG
    GUI::Begin("SnakeBehavior_Move");
    GUI::SliderFloat("deg", &rotateDegree_, 0.f, 360.f);
    GUI::End();

    if (KEYS::IsDown(DIK_9))
    {
        commonInfo_->axes_ = Axis3::Initialize();
    }
#endif // _DEBUG

    // タイマーの更新
    timer_randomWalk_.Update();

    // 回転移動処理
    float rad = Math::Function::ToRadian(rotateDegree_);
    RotateDirection(rad);

    Move();

    // タイマーの進行割合が100%なら
    const float rate = timer_randomWalk_.GetTimeRate();
    // 休憩フラグを立てる。
    if (rate >= 1.f) { is_needRest_ = true; }
}

void SnakeBehavior_Move::RequirementCheck(void)
{
    // 決められた時間活動した。
    if (is_needRest_)
    {
        // 蛇の振る舞いをIDLEへ変更
        nextBehavior_ = SnakeBehavior::IDLE;
        return;
    }

    // 卵の存在を検知した。
    if (commonInfo_->is_detectEgg_)
    {
        // 蛇の振る舞いをSNEAKへ変更
        nextBehavior_ = SnakeBehavior::SNEAK;
        return;
    }
}

void SnakeBehavior_Sneak::Initialize(Snake* arg_snakePtr)
{
    ISnakeBehavior::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::SNEAK;
}

void SnakeBehavior_Sneak::Entry(void)
{
    // 遷移時の正面ベクトルを記録
    vec3_entryForward_ = commonInfo_->axes_.forward;
    commonInfo_->axes_.forward = commonInfo_->vec3_toEgg_;

    // タイマー起動
    timer_rotateDirection_.Start(SnakeCommonInfomation::kTimer_rotateDirection_basic_);
}

void SnakeBehavior_Sneak::Execute(void)
{
    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 卵に接近する。。
    ApproachEgg();

    // 姿勢再計算
    Process_ReCulculatePosture();
}

void SnakeBehavior_Sneak::Move(void)
{
    // 重力処理（垂直方向の移動量）
    Process_Gravity();
    // 正面への移動（平行方向への移動）
    const Vector3 velocity_horizontal = commonInfo_->axes_.forward * commonInfo_->kMoveSpd_sneak_;

    // 移動総量計算
    const Vector3& velocity_total = Process_CalculateVelocity(velocity_horizontal);
    // 座標更新
    commonInfo_->transform_.position += velocity_total;
}

void SnakeBehavior_Sneak::ApproachEgg(void)
{
    // タイマーの更新
    timer_rotateDirection_.Update();

    // 正面ベクトルをイージングで捻じ曲げる
    //const float rate = timer_rotateDirection_.GetTimeRate();
    //commonInfo_->axes_.forward = Math::Ease3::EaseInSin(rate, vec3_entryForward_, commonInfo_->vec3_toEgg_); // EaseInOutSine
    commonInfo_->axes_.forward = commonInfo_->vec3_toEgg_;

    Move();
}

void SnakeBehavior_Sneak::RequirementCheck(void)
{
    // 卵を見失った。
    if (commonInfo_->is_detectEgg_ == false)
    {
        // 蛇の振る舞いをSNEAKへ変更
        nextBehavior_ = SnakeBehavior::IDLE;
        return;
    }
}

// -------------------------------------------------------------------------------------
std::unique_ptr<ISnakeBehavior> SnakeBehaviorFactory::Create(SnakeBehavior arg_behavior, Snake* arg_snakePtr)
{
    // 生成されたbehaviorを収納する変数
    std::unique_ptr<ISnakeBehavior> behavior;

    // 生成
    if (arg_behavior == SnakeBehavior::IDLE) { behavior = std::make_unique<SnakeBehavior_Idle>(); }
    else if (arg_behavior == SnakeBehavior::MOVE) { behavior = std::make_unique<SnakeBehavior_Move>(); }
    else if (arg_behavior == SnakeBehavior::SNEAK) { behavior = std::make_unique<SnakeBehavior_Sneak>(); }

    // 初期化関数の実行
    behavior->Initialize(arg_snakePtr);

    // リターン
    return std::move(behavior);
}


// -------------------------------------------------------------------------------------
void SnakeBehaviorMachine::Initialize(Snake* arg_snakePtr, SnakeBehavior arg_behavior)
{
    // ptrを受け取る
    snakePtr_ = arg_snakePtr;

    // 一番最初の振る舞いを生成。
    behaviorPtr_ = behaviorFactory_.Create(arg_behavior, snakePtr_);
    // マシンの初期化では遷移時初期化関数も呼び出す。
    behaviorPtr_->Entry();
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
