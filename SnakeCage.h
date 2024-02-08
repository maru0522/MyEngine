#pragma once
#include "Object3D.h"
#include "CollisionManager.h"
#include "CollisionPrimitive.h"
#include "Planet.h"

class SnakeCage final : public Object3D
{
public:
    //>> 定義
    const float kCollisionRadius_ = 3.f; // 当たり判定球の半径
    const float kModelScale_ = 3.f;      // モデルの拡縮倍率
    const float kGravity_ = 0.1f;        // 重力

    //>> 関数
    SnakeCage(void) = default;
    ~SnakeCage(void) override = default;

    // 初期化
    void Initialize(CollisionManager* arg_colMPtr, LightManager* arg_lightManagerPtr, Planet* arg_planetPtr);
    // 更新
    void Update(void) override;
    // 描画
    void Draw(void) override;
    // 終了
    void Finalize(void);

private:
    void AdaptPosture(void);

    void OnCollision(void);
    void OnTrigger(void);

    //>> 変数
    CollisionManager* colMPtr_;                             // 当たり判定マネージャー
    CollisionPrimitive::SphereCollider sphere_collision_;   // 当たり判定コライダー

    int32_t circleShadows_num_;       // 丸影の仕様番号
    LightManager* lightMPtr_;         // ライトマネージャー
    Planet* planetPtr_;               // 影をつける星

    float velocity_vertical_;         // 垂直方向の移動量 ※重力によって引き寄せられる力の大きさ
    Transform transform_;             // 座標など
    Vector3 vec3_newUp_;              // 新規上ベクトル
    Axis3 posture_;                   // 姿勢

    bool is_inCage_;                  // 蛇が中にいる

public:
    //>> setter
    void SetPosition(const Vector3& arg_pos) { transform_.position = arg_pos; }

    //>> getter
    bool GetIsInCage(void) { return is_inCage_; }
    Vector3* GetPosPtr(void) { return &transform_.position; }
};
