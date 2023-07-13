#include "Planet.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Planet::Planet(void)
{
    CollisionManager::GetInstance()->Register(&surface_);
    CollisionManager::GetInstance()->Register(&gravityArea_);
    surface_.SetID("terrainSurface");
    gravityArea_.SetID("gravityArea");
    surface_.SetOnCollision(std::bind(&Planet::OnCollision, this));
    gravityArea_.SetOnCollision(std::bind(&Planet::OnCollision, this));

    body_->GetCoordinatePtr()->SetPosition({ 0,0,0 });
    body_->GetCoordinatePtr()->SetScale({ 5,5,5 });
    surface_.radius = 1.f * 5;
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
