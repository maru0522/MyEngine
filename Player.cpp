#include "Player.h"
#include "Input.h"
#include "CollisionManager.h"

Player::Player(void)
{
    //CollisionManager* colMPtr = CollisionManager::GetInstance();
    //colMPtr->AddColList(ColliderFactory::Shape::SPHERE);
    //sphereCollider_ = colMPtr->GetListBackColPtr();

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

    Quaternion right = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(right);
    Quaternion forward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(forward);
    coordinate_.SetAxisUp(coordinate_.GetUpVec().ExtractVector3().Normalize());

    // À•WŒvŽZ
    Vector3 gravity = -coordinate_.GetUpVec().ExtractVector3();
    gravity *= 0.4f;

    Vector3 moveVec = { 0.f,0.f,0.f };
    if (KEYS::IsDown(DIK_W)) moveVec += forward.ExtractVector3();
    if (KEYS::IsDown(DIK_S)) moveVec -= forward.ExtractVector3();
    if (KEYS::IsDown(DIK_A)) moveVec -= right.ExtractVector3();
    if (KEYS::IsDown(DIK_D)) moveVec += right.ExtractVector3();

    Vector3 velocity = { 0.f,0.f,0.f };
    if (KEYS::IsDown(DIK_SPACE))
    {
        //  ‚¶‚á‚ñ‚Õ
        velocity += coordinate_.GetUpVec().ExtractVector3().Normalize() * kJumpPower_;
    }
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += gravity;

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
