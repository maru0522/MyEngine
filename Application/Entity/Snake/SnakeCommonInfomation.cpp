#include "SnakeCommonInfomation.h"

const float SnakeCommonInfomation::kRadius_col_{ 1.f };                             // 当たり判定の半径
const float SnakeCommonInfomation::kModelScale_{ 2.5f };                            // モデルの拡縮倍率
const float SnakeCommonInfomation::kRadius_detectEgg_{ 25.f };                      // 卵検知の半径
const float SnakeCommonInfomation::kRadius_detectPlayer_{ 25.f };                   // プレイヤー検知の半径
const float SnakeCommonInfomation::kRadius_detectHoleTriggerCollider_{ 20.f };      // 穴の起動用コライダー検知の半径
const float SnakeCommonInfomation::kDetectRadius_escape_{ 10.f };                   // さらにこの半径内に入ったら、遠ざかる挙動
const float SnakeCommonInfomation::kJumpPower_{ 0.4f };                             // ジャンプ力


const float SnakeCommonInfomation::kGravity_{ 0.025f };                             // かかる重力値
const float SnakeCommonInfomation::kMoveDist_{ 30.f };                              // プレイヤーを検知した地点からどのくらいの距離移動するのか


const float SnakeCommonInfomation::kTimer_rest_min_{2.f};              // 休憩時間の最小値
const float SnakeCommonInfomation::kTimer_rest_max_{5.f};              // 休憩時間の最大値

const float SnakeCommonInfomation::kMoveSpd_default_{ 0.24f };         // 普段の移動速度
const float SnakeCommonInfomation::kTimer_randomWalk_min_{ 6.f };      // ランダムに移動し続ける時間の最小値
const float SnakeCommonInfomation::kTimer_randomWalk_max_{ 12.f };     // ランダムに移動し続ける時間の最大値 ※最小~最大の間でランダムに決定される。
const float SnakeCommonInfomation::kDegree_randomWalk_min_{ 0.1f };    // 方向転換時の角度の最小値
const float SnakeCommonInfomation::kDegree_randomWalk_max_{ 1.f };     // 方向転換時の角度の最大値 ※最小~最大の間でランダムに決定される。

const float SnakeCommonInfomation::kMoveSpd_moveStomach_{ 0.04f };     // 卵消化完了にかかる時間
const float SnakeCommonInfomation::kTimer_completeEateEgg_{ 1500.f };  // 卵消化完了にかかる時間

const float SnakeCommonInfomation::kMoveSpd_sneak_{ 0.08f };                 // 卵にこっそり近づくときの移動速度
const float SnakeCommonInfomation::kTimer_rotateDirection_basic_{1.f};       // 卵発見時にそちらの方を向くのにかかる時間

const float SnakeCommonInfomation::kMoveSpd_escape_{ 0.32f };                // プレイヤーから逃げる時の移動速度
const float SnakeCommonInfomation::kDistance_escapePlayer_{ 20.f };          // プレイヤーから逃げる時の移動速度

const float SnakeCommonInfomation::kMoveSpd_escapeStomach_{ 0.07f };         // 卵消化中の移動速度

const float SnakeCommonInfomation::kMoveSpd_leaveEgg_{ 0.04f };        // 鶏の巣から遠ざかる時の移動速度
const float SnakeCommonInfomation::kDistance_leaveEgg_{ 40.f };        // どの程度の距離卵から離れるか