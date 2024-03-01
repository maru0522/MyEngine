#pragma once
#include "Object3D.h"
#include "CollisionManager.h"
#include "Transform.h"
#include "GamePlanet.h"
#include "Snake.h"

class ChickenEgg
{
public:
    //>> 定義
    const float kGravity_ = 0.1f;            // 重力
    const float kModelScale_ = 2.5f;         // モデルの拡縮倍率
    const float kRadius_col_ = 2.2f;         // 卵の当たり判定の半径
    const float kRadius_detectSnake_ = 30.f; // 卵の蛇検知半径
    static const int32_t skMaxCount_ = 5;    // 卵の数

    //>> 関数
    ChickenEgg(void) = default;
    ~ChickenEgg(void) { Finalize(); }

    // 初期化処理や、ptrの受け取り
    void Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr_, Planet* arg_planetPtr_);
    void Update(void);
    void Draw(void);

    // 蛇側が、鶏卵に接触した際に卵の数を減らすための関数 ※同F接触時に、1回分しかカウントされないバグ回避のため向こう側で使用する
    friend void Snake::SnakeRobChickenEgg(void);
private:

    // 鶏の卵が1つ以上残っているか
    bool IsChikenEgg(void) { return eggNum_ >= 1; }
    void Finalize(void);
    void AdaptPosture(void);

    void OnCollision_Col(void);
    void OnCollision_DetectSnake(void);

    //>> 変数
    std::array<Object3D, skMaxCount_> model_; // 卵のモデル。要素数に応じて数が変化。
    int32_t eggNum_ = skMaxCount_;            // 卵の数 ≒ HP
    int32_t snakeCount_{};                    // 卵付近にいる蛇の数。

    CollisionManager* colMPtr_;                             // 当たり判定マネージャー
    CollisionPrimitive::SphereCollider sphere_collision_;   // 当たり判定コライダー
    CollisionPrimitive::SphereCollider sphere_detectSnake_; // 蛇検知コライダー

    int32_t circleShadows_num_;       // 丸影の仕様番号
    LightManager* lightMPtr_;         // ライトマネージャー
    Planet* planetPtr_;               // 影をつける星

    TransformMatrix transformMatrix_; // ワールド行列など
    float velocity_vertical_;         // 垂直方向の移動量 ※重力によって引き寄せられる力の大きさ
    Transform transform_;             // 座標など
    Vector3 vec3_newUp_;              // 新規上ベクトル
    Axis3 posture_;                   // 姿勢

public:
    //>> getter
    int32_t GetApproachingEggSnakes(void) { return snakeCount_; }
};
