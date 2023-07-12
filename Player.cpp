#include "Player.h"
#include "Input.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Player::Player(void)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);

    sphereCollider_.radius = kRadius_;
    coordinate_.SetPosition({ 0,20,0 }); // �����ʒu
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
    // --- 1Frame�x�� ---
    OnCollision();
    appearance_->SetCoordinate(coordinate_);
    appearance_->Update();
    // ------------------

    sphereCollider_.center = coordinate_.GetPosition();

    Quaternion right = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(right);
    Quaternion forward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(forward);
    coordinate_.SetAxisUp(coordinate_.GetUpVec().ExtractVector3().Normalize());

    // ���W�v�Z
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
        //  ������
        velocity += coordinate_.GetUpVec().ExtractVector3().Normalize() * kJumpPower_;
    }
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += gravity;

    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos);
}

void Player::Draw(void)
{
    appearance_->Draw("Resources/red1x1.png");
}

void Player::OnCollision(void)
{
    if (sphereCollider_.GetIsHit())
    {
        GUI::Begin("phit");
        ImGui::Text("Hit!");
        GUI::End();
        sphereCollider_.SetIsHit(false);

        // �{���͋���d�̓G���A���ɓ����Ă�ꍇ�ɍs�������B
        Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        coordinate_.SetAxisUp(center2PlayerVec.Normalize());

        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��H�H
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - sphereCollider_.GetColInfo().f - sphereCollider_.radius;

        Vector3 currentPos = coordinate_.GetPosition();
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // ���K�����ꂽ������v���C���[�܂ł̃x�N�g�� * �߂荞�݋���
        currentPos += center2PlayerVec.Normalize() * -diff; // �������}�C�i�X�����Œl���]

        coordinate_.SetPosition(currentPos);
    }
}
