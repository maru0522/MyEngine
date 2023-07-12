#include "Planet.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Planet::Planet(void)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);

    body_->GetCoordinatePtr()->SetPosition({ 0,0,0 });
    body_->GetCoordinatePtr()->SetScale({ 5,5,5 });
    sphereCollider_.radius = 1.f * 5;
}

void Planet::Update(void)
{
    body_->Update();

    sphereCollider_.center = body_->GetCoordinatePtr()->GetPosition();

    if (sphereCollider_.GetIsHit())
    {
        GUI::Begin("planetHit");
        ImGui::Text("Hit!!!!!");
        GUI::End();
    }
}

void Planet::Draw(void)
{
    body_->Draw();
}
