#include "Player.h"
#include "Input.h"

Player::Player(void)
{
    sphereCollider_.radius = kRadius_;
    coordinate_.SetPosition({ 0,20,0 }); // ‰ŠúˆÊ’u
    coordinate_.SetAxisForward({ 0,0,1,0 });
    coordinate_.SetAxisRight({ 1,0,0,0 });
    coordinate_.SetAxisUp({ 0,1,0,0 });
    coordinate_.SetIsPriority(true);

    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec());
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec());
    appearance_->GetCoordinatePtr()->SetIsPriority(true);
}

void Player::Update(void)
{
    sphereCollider_.center = coordinate_.GetPosition();

    //Quaternion right = Math::QuaternionF::CrossVector3Part(body_->coordinate_.GetUpVec(), body_->coordinate_.GetForwardVec(), body_->coordinate_.GetRightVec().w);
    //body_->coordinate_.SetAxisRight(right.Normalize());
    //Quaternion forward = Math::QuaternionF::CrossVector3Part(right.Normalize(), body_->coordinate_.GetUpVec());
    //body_->coordinate_.SetAxisForward(forward.Normalize());
    //body_->coordinate_.SetAxisUp(body_->coordinate_.GetUpVec().Normalize());

    //rightVec_ = Math::Vec3::Cross(upVec_, forwardVec_);
    //rightVec_ = rightVec_.Normalize();
    //forwardVec_ = Math::Vec3::Cross(rightVec_, upVec_);
    //forwardVec_ = forwardVec_.Normalize();
    //upVec_ = upVec_.Normalize();

    Quaternion right = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(right);
    Quaternion forward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(forward);
    coordinate_.SetAxisUp(coordinate_.GetUpVec().ExtractVector3().Normalize());

    // À•WŒvŽZ
    Vector3 gravity = -coordinate_.GetUpVec().ExtractVector3();
    gravity *= 0.4f;

    //velocity_ = { 0,0,0 };
    //if (KEYS::IsDown(DIK_W)) velocity_ += forward.ExtractVector3();
    //if (KEYS::IsDown(DIK_S)) velocity_ -= forward.ExtractVector3();
    //if (KEYS::IsDown(DIK_A)) velocity_ -= right.ExtractVector3();
    //if (KEYS::IsDown(DIK_D)) velocity_ += right.ExtractVector3();

    Vector3 velocity = { 0.f,0.f,0.f };
    velocity += gravity;
    if (KEYS::IsDown(DIK_W)) velocity += forward.ExtractVector3();
    if (KEYS::IsDown(DIK_S)) velocity -= forward.ExtractVector3();
    if (KEYS::IsDown(DIK_A)) velocity -= right.ExtractVector3();
    if (KEYS::IsDown(DIK_D)) velocity += right.ExtractVector3();
    if (KEYS::IsDown(DIK_SPACE)) velocity += coordinate_.GetUpVec().ExtractVector3();

    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;

    coordinate_.SetPosition(currentPos);

    appearance_->SetCoordinate(coordinate_);
    appearance_->Update();
}

void Player::Draw(void)
{
    appearance_->Draw("Resources/red1x1.png");
}
