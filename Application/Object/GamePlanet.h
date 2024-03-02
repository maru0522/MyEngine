#pragma once
#include <memory>
#include "IPlanet.h"
#include "CollisionPrimitive.h"
#include "TerrainSurfaceCollider.h"
/**
 * @file GamePlanet.h
 * @brief チュートリアル用の島クラス
 */

class GamePlanet final : IPlanet
{
public:
    // 関数
    GamePlanet(void) = default;
    ~GamePlanet(void) override = default;

    void Initialize(CollisionManager* arg_colMPtr) override;
    void Update(void) override;
    void Draw(void) override;
    void Finalize(void) override;

    CollisionPrimitive::SphereCollider repelCameraArea_;

private:
    void OnCollision(void) override;

    // 変数
    std::array<std::unique_ptr<TerrainSurfaceCollider>,3> addCols_;
};
