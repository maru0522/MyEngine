#include "Planet.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Planet::Planet(void)
{
    // マネージャーに登録
    CollisionManager::GetInstance()->Register(&surface_);
    CollisionManager::GetInstance()->Register(&gravityArea_);
    CollisionManager::GetInstance()->Register(&repelCameraArea_);

    // 各コライダーの名前を設定
    surface_.SetID("terrainSurface");
    gravityArea_.SetID("gravityArea");
    repelCameraArea_.SetID("repelCameraArea");

    // OnCollisionの設定
    surface_.SetOnCollision(std::bind(&Planet::OnCollision, this));
    gravityArea_.SetOnCollision(std::bind(&Planet::OnCollision, this));
    repelCameraArea_.SetOnCollision(std::bind(&Planet::OnCollision, this));

    // 星自体の座標とスケールの設定
    body_->GetCoordinatePtr()->SetPosition({ 0,0,0 });
    body_->GetCoordinatePtr()->SetScale({ kScale_,kScale_,kScale_ });

    // 各コライダーの半径を設定
    surface_.radius = 1.f * kScale_;
    gravityArea_.radius = 1.f * kGravityArea_;
    repelCameraArea_.radius = 1.f * kRepelCameraArea_;
}

void Planet::Update(void)
{
    body_->Update();

    surface_.center = body_->GetCoordinatePtr()->GetPosition();
}

void Planet::Draw(void)
{
    body_->Draw();
}
