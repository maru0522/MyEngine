#include "Planet.h"

Planet::Planet(void)
{
    body_->coordinate_.SetPosition({ 0,0,0 });
    body_->coordinate_.SetScale({ 5,5,5 });
    sphereCollider_.radius = 1.f * 5;
}

void Planet::Update(void)
{
    body_->Update();

    sphereCollider_.center = body_->coordinate_.GetPosition();
}

void Planet::Draw(void)
{
    body_->Draw();
}
