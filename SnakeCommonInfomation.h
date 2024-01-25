#pragma once
#include "Timer.h"
#include "Transform.h"

struct SnakeCommonInfomation final
{
public:
    //>> 定義
    static const float kRadius_col_;               // 当たり判定の半径
    static const float kRadius_detectEgg_;         // 卵検知の半径
    static const float kRadius_detectPlayer_;      // プレイヤー検知の半径
    static const float kDetectRadius_escape_;      // さらにこの半径内に入ったら、遠ざかる挙動
    static const float kJumpPower_;                // ジャンプ力

    static const float kMoveSpd_sneak_;            // 卵にこっそり近づくときの移動速度
    static const float kMoveSpd_default_;          // 普段の移動速度
    static const float kMoveSpd_escape_;           // プレイヤーから逃げる時の移動速度

    static const float kGravity_;                  // かかる重力値
    static const float kMoveDist_;                 // プレイヤーを検知した地点からどのくらいの距離移動するのか

    static const float kTimer_randomWalk_default_; // ランダムに移動し続ける時間の初期値
    static const float kTimer_randomWalk_min_;     // ランダムに移動し続ける時間の最小値
    static const float kTimer_randomWalk_max_;     // ランダムに移動し続ける時間の最大値 ※最小~最大の間でランダムに決定される。
    static const float kDegree_randomWalk_min_;    // ランダムに移動し続ける際の角速度の最小値
    static const float kDegree_randomWalk_max_;    // ランダムに移動し続ける際の角速度の最大値 ※最小~最大の間でランダムに決定される。

    //>> 変数
    DeltaTimer timer_changeDirInterval_;
    bool is_detectEgg_{};

    TransformMatrix transformMatrix_; // 各ワールド行列
    Transform transform_;             // 座標等

    Axis3 axes_;                      // 姿勢
    Axis3 axes_old_;                  // 1F前の姿勢
    Vector3 vec3_newUp_;              // 新規姿勢の上ベクトル
    Vector3 vec3_moveDirection_;      // 移動する方向。


    float moveDist_; // 移動する距離。
    float velocity_vertical_{};


    bool is_landing_;
    bool is_detect_; // プレイヤーを検知したか
};

