#include "Player.h"
#include "Input.h"

Player::Player(void)
{
    body_->coordinate_.SetPosition({ 0,20,0 });
    sphereCollider_.radius = 1.f;
    upVec_ = { 0,1,0 };
}

void Player::Update(void)
{
    body_->Update();

    sphereCollider_.center = body_->coordinate_.GetPosition();

    // À•WŒvŽZ
    Vector3 gravity = -upVec_;
    gravity *= 0.1f;

    Vector3 currentPos = body_->coordinate_.GetPosition();
    currentPos += gravity;

    body_->coordinate_.SetPosition(currentPos);
}

void Player::Draw(void)
{
    body_->Draw("Resources/red1x1.png");
}
