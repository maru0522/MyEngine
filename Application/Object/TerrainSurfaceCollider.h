#pragma once
#include "CollisionManager.h"
#include "SphereCollider.h"
#include "Object3D.h"
/**
 * @file TerrainSurfaceCollider.h
 * @brief チュートリアル島の凹凸用簡易当たり判定球
 */

class TerrainSurfaceCollider : public Object3D
{
public:
    //>> 関数
    TerrainSurfaceCollider(CollisionManager* arg_colMptr) : TerrainSurfaceCollider(arg_colMptr, Vector3{ 0,0,0 }, 1.f) {}
    TerrainSurfaceCollider(CollisionManager* arg_colMptr, const Vector3& arg_pos, float arg_radius);
    ~TerrainSurfaceCollider(void);

    void Update(void);
    void Draw(void);

private:
    //>> 変数
    CollisionManager* colMPtr_;
    CollisionPrimitive::SphereCollider terrainSurface_;

public:
    //>> setter
    void SetPos(const Vector3& arg_pos) { terrainSurface_.center = arg_pos; }
    void SetRadius(float arg_radius) { terrainSurface_.radius = arg_radius; }
};

