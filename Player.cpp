#include "Player.h"
#include "Input.h"

Player::Player(void)
{
    sphereCollider_.radius = 1.f;
    body_->coordinate_.SetPosition({ 0,20,0 });
    body_->coordinate_.SetAxisForward({ 0,0,1,0 });
    body_->coordinate_.SetAxisRight({ 1,0,0,0 });
    body_->coordinate_.SetAxisUp({ 0,1,0,0 });
    body_->coordinate_.SetIsPriority(true);
}

void Player::Update(void)
{
    body_->Update();

    sphereCollider_.center = body_->coordinate_.GetPosition();

    //Quaternion right = Math::QuaternionF::CrossVector3Part(body_->coordinate_.GetUpVec(), body_->coordinate_.GetForwardVec(), body_->coordinate_.GetRightVec().w);
    //body_->coordinate_.SetAxisRight(right.Normalize());
    //Quaternion forward = Math::QuaternionF::CrossVector3Part(right.Normalize(), body_->coordinate_.GetUpVec());
    //body_->coordinate_.SetAxisForward(forward.Normalize());
    //body_->coordinate_.SetAxisUp(body_->coordinate_.GetUpVec().Normalize());

    rightVec_ = Math::Vec3::Cross(upVec_, forwardVec_);
    rightVec_ = rightVec_.Normalize();
    forwardVec_ = Math::Vec3::Cross(rightVec_, upVec_);
    forwardVec_ = forwardVec_.Normalize();
    upVec_ = upVec_.Normalize();

    // À•WŒvŽZ
    Vector3 gravity = -upVec_;
    gravity *= 0.1f;

    //velocity_ = { 0,0,0 };
    //if (KEYS::IsDown(DIK_W)) velocity_ += forward.ExtractVector3();
    //if (KEYS::IsDown(DIK_S)) velocity_ -= forward.ExtractVector3();
    //if (KEYS::IsDown(DIK_A)) velocity_ -= right.ExtractVector3();
    //if (KEYS::IsDown(DIK_D)) velocity_ += right.ExtractVector3();

    velocity_ = { 0.f,0.f,0.f };
    if (KEYS::IsDown(DIK_W)) velocity_ += forwardVec_;
    if (KEYS::IsDown(DIK_S)) velocity_ -= forwardVec_;
    if (KEYS::IsDown(DIK_A)) velocity_ -= rightVec_;
    if (KEYS::IsDown(DIK_D)) velocity_ += rightVec_;

    Vector3 currentPos = body_->coordinate_.GetPosition();
    currentPos += gravity;
    currentPos += velocity_;

    body_->coordinate_.SetPosition(currentPos);
}

void Player::Draw(void)
{
    body_->Draw("Resources/red1x1.png");
}
