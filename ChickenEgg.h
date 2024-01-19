#pragma once
#include "Object3D.h"
#include "CollisionManager.h"
#include "Transform.h"
#include "Planet.h"

class ChickenEgg
{
public:
    //>> 定義
    const float kRadius_col_ = 3.f;         // 卵の当たり判定の半径
    const float kRadius_detectSnake_ = 3.f; // 卵の蛇検知半径

    //>> 関数
    ChickenEgg(void) = default;
    ~ChickenEgg(void) { Finalize(); }

    // 初期化処理や、ptrの受け取り
    void Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr_, Planet* arg_planetPtr_);
    void Update(void);
    void Draw(void);

private:
    void Finalize(void);
    void AdaptPosture(void);

    void OnCollision_Col(void);
    void OnCollision_DetectSnake(void);

    //>> 変数
    CollisionManager* colMPtr_;                             // 当たり判定マネージャー
    CollisionPrimitive::SphereCollider sphere_collision_;   // 当たり判定コライダー
    CollisionPrimitive::SphereCollider sphere_detectSnake_; // 蛇検知コライダー

    int32_t circleShadows_num_;       // 丸影の仕様番号
    LightManager* lightMPtr_;         // ライトマネージャー
    Planet* planetPtr_;               // 影をつける星

    TransformMatrix transformMatrix_; // ワールド行列など
    Transform transform_;             // 座標など
    Vector3 vec3_newUp_;              // 新規上ベクトル
    Axis3 posture_;                   // 姿勢
};

