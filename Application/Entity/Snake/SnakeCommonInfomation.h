#pragma once
#include "Timer.h"
#include "Transform.h"

struct SnakeCommonInfomation final
{
public:
    //>> 定義
    static const float kRadius_col_;                        // 当たり判定の半径
    static const float kModelScale_;                        // モデルの拡縮倍率
    static const float kRadius_detectEgg_;                  // 卵検知の半径
    static const float kRadius_detectHoleTriggerCollider_;  // 穴の起動用コライダー検知の半径
    static const float kRadius_detectPlayer_;               // プレイヤー検知の半径
    static const float kDetectRadius_escape_;               // さらにこの半径内に入ったら、遠ざかる挙動
    static const float kJumpPower_;                         // ジャンプ力


    static const float kGravity_;                           // かかる重力値
    static const float kMoveDist_;                          // プレイヤーを検知した地点からどのくらいの距離移動するのか


    // SnakeBehavior::IDLE
    static const float kTimer_rest_min_;           // 休憩時間の最小値
    static const float kTimer_rest_max_;           // 休憩時間の最大値

    // SnakeBehavior::MOVE
    static const float kMoveSpd_default_;          // 普段の移動速度
    static const float kTimer_randomWalk_min_;     // ランダムに移動し続ける時間の最小値
    static const float kTimer_randomWalk_max_;     // ランダムに移動し続ける時間の最大値 ※最小~最大の間でランダムに決定される。
    static const float kDegree_randomWalk_min_;    // ランダムに移動し続ける際の角速度の最小値
    static const float kDegree_randomWalk_max_;    // ランダムに移動し続ける際の角速度の最大値 ※最小~最大の間でランダムに決定される。

    // SnakeBehavior::MOVE_STOMACH
    static const float kMoveSpd_moveStomach_;          // 卵消化中の移動速度
    static const float kTimer_completeEateEgg_;    // 卵消化完了にかかる時間

    // SnakeBehavior::SNEAK
    static const float kMoveSpd_sneak_;                  // 卵にこっそり近づくときの移動速度
    static const float kTimer_rotateDirection_basic_;    // 卵発見時にそちらの方を向くのにかかる時間

    // SnakeBehavior::ESCAPE
    static const float kMoveSpd_escape_;                 // プレイヤーから逃げる時の移動速度
    static const float kDistance_escapePlayer_;          // どの程度の距離プレイヤーから逃げるか

    // SnakeBehavior::ESCAPE_STOMACH
    static const float kMoveSpd_escapeStomach_;          // 卵消化中の移動速度

    // SnakeBehavior::LEAVE_EGG
    static const float kMoveSpd_leaveEgg_;               // 鶏の巣から遠ざかる時の移動速度
    static const float kDistance_leaveEgg_;              // どの程度の距離卵から離れるか


    //>> 変数
    Vector3 vec3_toEgg_;                 // 卵への方向
    bool is_detectEgg_{};                // 卵を検知したか

    Vector3 vec3_toHoleTriggerCollider_; // 星の穴のイベント起動コライダーへの方向。

    Vector3 vec3_toPlayer_;              // プレイヤーへの方向
    bool is_detectPlayer_{};             // プレイヤーを検知したか
    bool is_touchPlayer_{};              // プレイヤーへ触れたか

    Vector3 pos_chickenEgg_;             // 鶏の巣の座標

    bool is_eatChickenEgg_;              // 卵を食べたか
    DeltaTimer timer_completeEatEgg_;    // 卵を消化しきるまでの時間

    bool is_touchCage_;                  // ケージに触れたか
    bool is_Caged_;                      // 収容されたか
    Vector3* pos_cage_;                  // ケージの座標

    bool is_landing_;


    TransformMatrix transformMatrix_; // 各ワールド行列
    Transform transform_;             // 座標等

    Axis3 axes_;                      // 姿勢
    Axis3 axes_old_;                  // 1F前の姿勢
    Vector3 vec3_newUp_;              // 新規姿勢の上ベクトル
    Vector3 vec3_moveDirection_;      // 移動する方向。


    float moveDist_; // 移動する距離。
    float velocity_vertical_{};
};

