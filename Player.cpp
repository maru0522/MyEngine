#include "Player.h"
#include "Input.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Player::Player(void)/* : isGrounded_(false)*/
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.SetOnCollision(std::bind(&Player::OnCollision, this));

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
    appearance_->SetCoordinate(coordinate_);
    appearance_->Update();
    // ------------------

    Quaternion right = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(right);
    Quaternion forward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(forward);
    coordinate_.SetAxisUp(coordinate_.GetUpVec().ExtractVector3().Normalize());

    // �ړ��x�N�g��
    Vector3 moveVec = { 0.f,0.f,0.f };
    if (KEYS::IsDown(DIK_W)) moveVec += forward.ExtractVector3();
    if (KEYS::IsDown(DIK_S)) moveVec -= forward.ExtractVector3();
    if (KEYS::IsDown(DIK_A)) moveVec -= right.ExtractVector3();
    if (KEYS::IsDown(DIK_D)) moveVec += right.ExtractVector3();

    // �d��
    Vector3 gravity = -coordinate_.GetUpVec().ExtractVector3().Normalize();
    gravity *= kGravity_;

    // �W�����v�x�N�g��
    if (KEYS::IsTrigger(DIK_SPACE))
    {
        //  ������
        jumpVec_ += coordinate_.GetUpVec().ExtractVector3().Normalize() * kJumpPower_;
        //isGrounded_ = false;
    }
    jumpVec_ += gravity;

    // �ړ���
    Vector3 velocity = { 0.f,0.f,0.f };
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec_;

    // ���W�X�V
    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos);

    // �R���C�_�[�X�V
    sphereCollider_.center = coordinate_.GetPosition();
}

void Player::Draw(void)
{
    appearance_->Draw("Resources/red1x1.png");
}

void Player::OnCollision(void)
{
    if (sphereCollider_.GetColInfo().id == "gravityArea")
    {
        // �{���͋���d�̓G���A���ɓ����Ă�ꍇ�ɍs�������B
        Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        coordinate_.SetAxisUp(center2PlayerVec.Normalize());
    }
    if (sphereCollider_.GetColInfo().id == "terrainSurface")
    {
        //isGrounded_ = true;
        jumpVec_ = { 0,0,0 };

        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��H�H
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

        Vector3 currentPos = coordinate_.GetPosition();
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // ���K�����ꂽ������v���C���[�܂ł̃x�N�g�� * �߂荞�݋���
        currentPos += coordinate_.GetUpVec().ExtractVector3().Normalize() * -diff; // �������}�C�i�X�����Œl���]

        coordinate_.SetPosition(currentPos);
    }
    else
    {
        //isGrounded_ = false;
    }
}
