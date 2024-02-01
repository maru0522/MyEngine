#pragma once
#include <memory>
#include "Object3D.h"
#include "CollisionPrimitive.h"
#include "TerrainSurfaceCollider.h"
/**
 * @file Planet.h
 * @brief チュートリアル用の島クラス
 */

class Planet
{
public:
    // 定数
    const float kScale_{ 50.f };
    const float kGravityArea_{ 500.f };
    const float kRepelCameraArea_{ 100.f };

    // 関数
    Planet(void) = default;
    ~Planet(void) = default;

    void Initialize(CollisionManager* arg_colMPtr);
    void Update(void);
    void Draw(void);
    void Finalize(void);

    Transform transform_;

    CollisionManager* colMPtr_{};
    CollisionPrimitive::SphereCollider surface_;
    CollisionPrimitive::SphereCollider gravityArea_;
    CollisionPrimitive::SphereCollider repelCameraArea_;
    bool isUV_;

private:
    void OnCollision(void) {}

    // 変数
    std::unique_ptr<Object3D> appearance_{ std::make_unique<Object3D>("Resources/model/planet/planet2.obj") };
    std::array<std::unique_ptr<TerrainSurfaceCollider>,3> addCols_;

public:
    // getter
    const Vector3& GetPosition(void) { return transform_.position; }

};
