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

void ISnakeBehavior::Move(float arg_speed)
{
    // 重力処理（垂直方向の移動量）
    Process_Gravity();
    // 正面への移動（平行方向への移動）
    const Vector3 velocity_horizontal = commonInfo_->axes_.forward * arg_speed;

    // 移動総量計算
    const Vector3& velocity_total = Process_CalculateVelocity(velocity_horizontal);
    // 座標更新
    commonInfo_->transform_.position += velocity_total;
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

void ISnakeBehavior::Process_RedefineForwardVec(void)
{
    // 現在の上ベクトルと右ベクトルから正面ベクトルを再定義
    // 正面ベクトルが、プレイヤーを検知した瞬間のままだと、移動するにつれ兎がつぶれる（姿勢が正常ではないため）
    const Vector3 forward = Math::Vec3::Cross(commonInfo_->axes_.right, commonInfo_->axes_.up);
    // 再定義したベクトルを正面ベクトルに設定
    commonInfo_->axes_.forward = forward;
    // 再定義したベクトルを移動方向ベクトルに設定。
    commonInfo_->vec3_moveDirection_ = forward;
}

void ISnakeBehavior::Process_RecalculatePosture(void)
{
    const Vector3& right = Math::Vec3::Cross(commonInfo_->axes_.up, commonInfo_->axes_.forward);
    const Vector3& forward = Math::Vec3::Cross(right, commonInfo_->axes_.up);

    commonInfo_->vec3_moveDirection_ = forward;
    commonInfo_->axes_.forward = forward;
    commonInfo_->axes_.right = right;
}
void ISnakeBehavior::Process_DebugGUI(void)
{
#ifdef _DEBUG
    GUI::Begin("snake_behavior");

    switch (nextBehavior_)
    {
    case SnakeBehavior::NONE:
        GUI::Text("next: NONE");
        break;
    case SnakeBehavior::IDLE:
        GUI::Text("next: IDLE");
        break;
    case SnakeBehavior::MOVE:
        GUI::Text("next: MOVE");
        break;
    case SnakeBehavior::MOVE_STOMACH:
        GUI::Text("next: MOVE_STOMACH");
        break;
    case SnakeBehavior::SNEAK:
        GUI::Text("next: SNEAK");
        break;
    case SnakeBehavior::ESCAPE:
        GUI::Text("next: ESCAPE");
        break;
    case SnakeBehavior::ESCAPE_STOMACH:
        GUI::Text("next: ESCAPE_STOMACH");
        break;
    case SnakeBehavior::LEAVE_EGG:
        GUI::Text("next: LEAVE_EGG");
        break;
    default:
        GUI::Text("next: unknown");
        break;
    }

    switch (currentBehavior_)
    {
    case SnakeBehavior::NONE:
        GUI::Text("current: NONE");
        break;
    case SnakeBehavior::IDLE:
        GUI::Text("current: IDLE");
        break;
    case SnakeBehavior::MOVE:
        GUI::Text("current: MOVE");
        break;
    case SnakeBehavior::MOVE_STOMACH:
        GUI::Text("current: MOVE_STOMACH");
        break;
    case SnakeBehavior::SNEAK:
        GUI::Text("current: SNEAK");
        break;
    case SnakeBehavior::ESCAPE:
        GUI::Text("current: ESCAPE");
        break;
    case SnakeBehavior::ESCAPE_STOMACH:
        GUI::Text("current: ESCAPE_STOMACH");
        break;
    case SnakeBehavior::LEAVE_EGG:
        GUI::Text("current: LEAVE_EGG");
        break;
    default:
        GUI::Text("current: unknown");
        break;
    }

    GUI::End();
#endif // _DEBUG
}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


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
    Process_DebugGUI();

    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 重力処理
    Process_Gravity();
    // 移動総量計算
    const Vector3& velocity_total = Process_CalculateVelocity();
    // 座標更新
    commonInfo_->transform_.position += velocity_total;

    // 姿勢再計算
    Process_RedefineForwardVec();

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

    // プレイヤーの存在を検知した
    if (commonInfo_->is_detectPlayer_)
    {
        // 蛇の振る舞いをESCAPEへ変更
        nextBehavior_ = SnakeBehavior::ESCAPE;
        return;
    }
}

// ---------------------------------------------------------------------------------------------
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
    Process_DebugGUI();

    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // ランダムに動き回る。
    RamdomWalk();

    // 姿勢再計算
    Process_RedefineForwardVec();
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

    ISnakeBehavior::Move(commonInfo_->kMoveSpd_default_);

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

    // プレイヤーの存在を検知した
    if (commonInfo_->is_detectPlayer_)
    {
        // 蛇の振る舞いをESCAPEへ変更
        nextBehavior_ = SnakeBehavior::ESCAPE;
        return;
    }
}

// ---------------------------------------------------------------------------------------------
void SnakeBehavior_MoveStomach::Initialize(Snake* arg_snakePtr)
{
    SnakeBehavior_Move::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::MOVE_STOMACH;
}

void SnakeBehavior_MoveStomach::Entry(void)
{
}

void SnakeBehavior_MoveStomach::Execute(void)
{
}

void SnakeBehavior_MoveStomach::RequirementCheck(void)
{
}

// ---------------------------------------------------------------------------------------------
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
    Process_DebugGUI();

    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 卵に接近する。。
    ApproachEgg();

    // 姿勢再計算
    Process_RecalculatePosture();
}

void SnakeBehavior_Sneak::ApproachEgg(void)
{
    // タイマーの更新
    timer_rotateDirection_.Update();

    // 正面ベクトルをイージングで捻じ曲げる
    //const float rate = timer_rotateDirection_.GetTimeRate();
    //commonInfo_->axes_.forward = Math::Ease3::EaseInSin(rate, vec3_entryForward_, commonInfo_->vec3_toEgg_); // EaseInOutSine
    commonInfo_->axes_.forward = commonInfo_->vec3_toEgg_;

    ISnakeBehavior::Move(commonInfo_->kMoveSpd_sneak_);
}

void SnakeBehavior_Sneak::RequirementCheck(void)
{
    // 卵を見失った。
    if (commonInfo_->is_detectEgg_ == false)
    {
        // 蛇の振る舞いをIDLEへ変更
        nextBehavior_ = SnakeBehavior::IDLE;
        return;
    }

    // プレイヤーの存在を検知した
    if (commonInfo_->is_detectPlayer_)
    {
        // 蛇の振る舞いをESCAPEへ変更
        nextBehavior_ = SnakeBehavior::ESCAPE;
        return;
    }

    if (commonInfo_->is_eatChickenEgg_)
    {
        // 蛇の振る舞いをLEAVE_EGGへ変更
        nextBehavior_ = SnakeBehavior::LEAVE_EGG;
        return;
    }
}

// ---------------------------------------------------------------------------------------------
void SnakeBehavior_Escape::Initialize(Snake* arg_snakePtr)
{
    ISnakeBehavior::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::ESCAPE;
}

void SnakeBehavior_Escape::Entry(void)
{
    // 遷移時の正面ベクトルを記録
    //vec3_entryForward_ = commonInfo_->axes_.forward;
    Vector3 vec3_escapePlayer = commonInfo_->vec3_toPlayer_ * -1;
    commonInfo_->axes_.forward = vec3_escapePlayer;
}

void SnakeBehavior_Escape::Execute(void)
{
    Process_DebugGUI();

    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 卵に接近する。。
    EscapePlayer(commonInfo_->kMoveSpd_escape_);

    // 姿勢再計算
    Process_RecalculatePosture();
}

void SnakeBehavior_Escape::EscapePlayer(float arg_moveSpd)
{
    // ESCAPE中にプレイヤーを再検知した際は正面ベクトルを更新
    if (commonInfo_->is_detectPlayer_)
    {
        // 逃走方向を算出し、正面ベクトルに適用
        Vector3 vec3_escapePlayer = commonInfo_->vec3_toPlayer_ * -1;
        commonInfo_->axes_.forward = vec3_escapePlayer;
    }

    // 移動処理
    ISnakeBehavior::Move(arg_moveSpd);

    // 逃走距離の合計に加算
    distance_escapePlayer_ += arg_moveSpd;
    // 逃走距離の合計が、規定値"kDistance_escapePlayer_"以上なら、is_enoughEscape をtrueにする
    if (distance_escapePlayer_ >= commonInfo_->kDistance_escapePlayer_) { is_enoughEscape_ = true; }
}

void SnakeBehavior_Escape::RequirementCheck(void)
{
    // プレイヤーを検知していない　&& 十分に逃げた
    if (commonInfo_->is_detectPlayer_ == false && is_enoughEscape_)
    {
        // 蛇の振る舞いをMOVEへ変更
        nextBehavior_ = SnakeBehavior::MOVE;
        return;
    }
}

// ---------------------------------------------------------------------------------------------
void SnakeBehavior_EscapeStomach::Initialize(Snake* arg_snakePtr)
{
    SnakeBehavior_Escape::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::ESCAPE_STOMACH;
}

void SnakeBehavior_EscapeStomach::Entry(void)
{
    // 遷移時の正面ベクトルを記録
    //vec3_entryForward_ = commonInfo_->axes_.forward;
    Vector3 vec3_escapePlayer = commonInfo_->vec3_toPlayer_ * -1;
    commonInfo_->axes_.forward = vec3_escapePlayer;
}

void SnakeBehavior_EscapeStomach::Execute(void)
{
    Process_DebugGUI();

    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 卵に接近する。。
    EscapePlayer(commonInfo_->kMoveSpd_stomach_);

    // 姿勢再計算
    Process_RecalculatePosture();
}

void SnakeBehavior_EscapeStomach::EscapePlayer(float arg_moveSpd)
{
    SnakeBehavior_Escape::EscapePlayer(arg_moveSpd);

    // 消化時間のタイマー更新
    commonInfo_->timer_completeEatEgg_.Update();
    // タイマーの進行割合
    const float rate = commonInfo_->timer_completeEatEgg_.GetTimeRate();
    // 進行割合が100%なら卵を食べているフラグをfalse
    if (rate >= 1.f) { commonInfo_->is_eatChickenEgg_ = false; }
}

void SnakeBehavior_EscapeStomach::RequirementCheck(void)
{
    // 卵を食べていない
    if (commonInfo_->is_eatChickenEgg_ == false)
    {
        // 蛇の振る舞いをMOVEへ変更
        nextBehavior_ = SnakeBehavior::MOVE;
        return;
    }
}

// ---------------------------------------------------------------------------------------------
void SnakeBehavior_LeaveEgg::Initialize(Snake* arg_snakePtr)
{
    ISnakeBehavior::Initialize(arg_snakePtr);

    nextBehavior_ = SnakeBehavior::NONE;
    currentBehavior_ = SnakeBehavior::LEAVE_EGG;
}

void SnakeBehavior_LeaveEgg::Entry(void)
{
    pos_chikenEgg_ = commonInfo_->pos_chickenEgg_;

    // タイマーの初期化と起動
    commonInfo_->timer_completeEatEgg_.Finish(true);
    commonInfo_->timer_completeEatEgg_.Start(commonInfo_->kTimer_completeEateEgg_);
}

void SnakeBehavior_LeaveEgg::Execute(void)
{
    Process_DebugGUI();

    // 姿勢の更新 + 右vec再計算
    Process_UpdatePosture();

    // 卵から遠ざかる
    LeaveChikenEgg();

    // 姿勢再計算
    Process_RecalculatePosture();
}

void SnakeBehavior_LeaveEgg::LeaveChikenEgg(void)
{
    // 卵から遠ざかる方向のベクトル
    const Vector3& vec3_egg2Snake = Vector3(commonInfo_->transform_.position - pos_chikenEgg_);
    // 卵から蛇までの距離
    const float distance = vec3_egg2Snake.Length();
    // 距離が、規定値"SnakeCommonInfomation::kDistance_leaveEgg_"より大きかったら
    if (distance > SnakeCommonInfomation::kDistance_leaveEgg_)
    {
        // 十分な距離があるかのフラグをtrue
        is_enoughLeave_ = true;
        // 終了
        return;
    }

    // 遠ざかる方向を正面ベクトルとして設定
    commonInfo_->axes_.forward = vec3_egg2Snake.Normalize();
    // 移動関数呼び出し
    ISnakeBehavior::Move(commonInfo_->kMoveSpd_leaveEgg_);
}

void SnakeBehavior_LeaveEgg::RequirementCheck(void)
{
    if (is_enoughLeave_)
    {
        // 蛇の振る舞いをMOVE_STOMACHへ変更
        nextBehavior_ = SnakeBehavior::MOVE_STOMACH;
        return;
    }

    // プレイヤーを検知
    if (commonInfo_->is_detectPlayer_)
    {
        // 蛇の振る舞いをESCAPE_STOMACHへ変更
        nextBehavior_ = SnakeBehavior::ESCAPE_STOMACH;
        return;
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
std::unique_ptr<ISnakeBehavior> SnakeBehaviorFactory::Create(SnakeBehavior arg_behavior, Snake* arg_snakePtr)
{
    // 生成されたbehaviorを収納する変数
    std::unique_ptr<ISnakeBehavior> behavior;

    // 生成
    if (arg_behavior == SnakeBehavior::IDLE) { behavior = std::make_unique<SnakeBehavior_Idle>(); }
    else if (arg_behavior == SnakeBehavior::MOVE) { behavior = std::make_unique<SnakeBehavior_Move>(); }
    else if (arg_behavior == SnakeBehavior::MOVE_STOMACH) { behavior = std::make_unique<SnakeBehavior_MoveStomach>(); }
    else if (arg_behavior == SnakeBehavior::SNEAK) { behavior = std::make_unique<SnakeBehavior_Sneak>(); }
    else if (arg_behavior == SnakeBehavior::ESCAPE) { behavior = std::make_unique<SnakeBehavior_Escape>(); }
    else if (arg_behavior == SnakeBehavior::ESCAPE_STOMACH) { behavior = std::make_unique<SnakeBehavior_EscapeStomach>(); }
    else if (arg_behavior == SnakeBehavior::LEAVE_EGG) { behavior = std::make_unique<SnakeBehavior_LeaveEgg>(); }

    // 初期化関数の実行
    behavior->Initialize(arg_snakePtr);

    // リターン
    return std::move(behavior);
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
