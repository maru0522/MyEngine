#include "Planet.h"

Planet::Planet(void)
{
    body_->GetCoordinatePtr()->SetPosition({ 0,0,0 });
    body_->GetCoordinatePtr()->SetScale({ 5,5,5 });
    sphereCollider_.radius = 1.f * 5;
}

void Planet::Update(void)
{
    body_->Update();

    sphereCollider_.center = body_->GetCoordinatePtr()->GetPosition();
}

void Planet::Draw(void)
{
    body_->Draw();
}
