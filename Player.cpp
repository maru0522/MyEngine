#include "Player.h"
#include "Input.h"

Player::Player(void)
{
    body_->coordinate_.SetPosition({ 0,20,0 });
    sphereCollider_.radius = 1.f;
    upVec_ = { 0,1,0 };
    forwardVec_ = { 0,0,1 };
    rightVec_ = { 1,0,0 };
}

void Player::Update(void)
{
    body_->Update();

    sphereCollider_.center = body_->coordinate_.GetPosition();

    rightVec_ = Math::Vector::Cross(upVec_, forwardVec_);
    rightVec_ = rightVec_.normalize();
    forwardVec_ = Math::Vector::Cross(rightVec_, upVec_);
    forwardVec_ = forwardVec_.normalize();
    upVec_ = upVec_.normalize();

    // À•WŒvŽZ
    Vector3 gravity = -upVec_;
    gravity *= 0.1f;

    velocity_ = { 0.f,0.f,0.f };
    velocity_ += gravity;

    if (KEYS::IsDown(DIK_W)) velocity_ += forwardVec_;
    if (KEYS::IsDown(DIK_S)) velocity_ -= forwardVec_;
    if (KEYS::IsDown(DIK_A)) velocity_ -= rightVec_;
    if (KEYS::IsDown(DIK_D)) velocity_ += rightVec_;

    Vector3 currentPos = body_->coordinate_.GetPosition();
    currentPos += velocity_;

    body_->coordinate_.SetPosition(currentPos);
}

void Player::Draw(void)
{
    body_->Draw("Resources/red1x1.png");
}
