#include "SnakeCommonInfomation.h"

const float SnakeCommonInfomation::kRadius_col_{ 1.f };                // 当たり判定の半径
const float SnakeCommonInfomation::kRadius_detectEgg_{ 30.f };         // 卵検知の半径
const float SnakeCommonInfomation::kRadius_detectPlayer_{ 30.f };      // プレイヤー検知の半径
const float SnakeCommonInfomation::kDetectRadius_escape_{ 10.f };      // さらにこの半径内に入ったら、遠ざかる挙動
const float SnakeCommonInfomation::kJumpPower_{ 0.4f };                // ジャンプ力

const float SnakeCommonInfomation::kMoveSpd_sneak_{ 0.15f };           // 卵にこっそり近づくときの移動速度
const float SnakeCommonInfomation::kMoveSpd_default_{ 0.24f };         // 普段の移動速度
const float SnakeCommonInfomation::kMoveSpd_escape_{ 0.38f };          // プレイヤーから逃げる時の移動速度

const float SnakeCommonInfomation::kGravity_{ 0.025f };                // かかる重力値
const float SnakeCommonInfomation::kMoveDist_{ 30.f };                 // プレイヤーを検知した地点からどのくらいの距離移動するのか


const float SnakeCommonInfomation::kTimer_rest_min_{2.f};              // 休憩時間の最小値
const float SnakeCommonInfomation::kTimer_rest_max_{5.f};              // 休憩時間の最大値

const float SnakeCommonInfomation::kTimer_randomWalk_min_{ 6.f };      // ランダムに移動し続ける時間の最小値
const float SnakeCommonInfomation::kTimer_randomWalk_max_{ 12.f };     // ランダムに移動し続ける時間の最大値 ※最小~最大の間でランダムに決定される。
const float SnakeCommonInfomation::kDegree_randomWalk_min_{ 0.1f };    // 方向転換時の角度の最小値
const float SnakeCommonInfomation::kDegree_randomWalk_max_{ 1.f };     // 方向転換時の角度の最大値 ※最小~最大の間でランダムに決定される。