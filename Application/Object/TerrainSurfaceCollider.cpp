#include "TerrainSurfaceCollider.h"

TerrainSurfaceCollider::TerrainSurfaceCollider(CollisionManager* arg_colMptr, const Vector3& arg_pos, float arg_radius)
    : Object3D("Resources/model/sphere/sphere.obj"), colMPtr_(arg_colMptr)
{
    // ID設定
    terrainSurface_.SetID("terrainSurface");
    // 中心点と半径の設定
    terrainSurface_.center = arg_pos;
    terrainSurface_.radius = arg_radius;

    // マネージャーへ登録
    arg_colMptr->Register(&terrainSurface_);
}

TerrainSurfaceCollider::~TerrainSurfaceCollider(void)
{
    // マネージャーから抹消
    colMPtr_->UnRegister(&terrainSurface_);
}

void TerrainSurfaceCollider::Update(void)
{
    Transform transform;
    transform.position = terrainSurface_.center;
    transform.scale = { terrainSurface_.radius, terrainSurface_.radius, terrainSurface_.radius };

    GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(transform);
    Object3D::Update();
}

void TerrainSurfaceCollider::Draw(void)
{
    Object3D::Draw();
}
