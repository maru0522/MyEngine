#include "Rabbit.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Rabbit::Rabbit(void)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);
    CollisionManager::GetInstance()->Register(&detectPlayerCollider_);

    sphereCollider_.SetID("rabbit");
    detectPlayerCollider_.SetID("rabbit_detectPlayer");

    sphereCollider_.SetOnCollision(std::bind(&Rabbit::OnCollision, this));
    detectPlayerCollider_.SetOnCollision(std::bind(&Rabbit::OnDetectPlayer, this));

    sphereCollider_.radius = kRadius_;
    detectPlayerCollider_.radius = kDetectRadius_;

    coordinate_.SetPosition({ 0,60,20 }); // �����ʒu
    coordinate_.SetAxisForward({ 0,0,1 });
    coordinate_.SetAxisRight({ 1,0,0 });
    coordinate_.SetAxisUp({ 0,1,0 });
    coordinate_.SetIsPriority(false);

    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec());
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec());
    appearance_->GetCoordinatePtr()->SetIsPriority(false);
}

void Rabbit::Update(void)
{
    // 1Frame�x���`����W���X�V ** ���W���m�肵����ɁA�����蔻�菈���ō��W��␳���邽�߁A1Frame�x�点�Ȃ��ƃK�N���\��������B
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec().Normalize());
    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->Update();

    static float sDetectRadius{ kDetectRadius_ };
    GUI::Begin("Rabbit");
    ImGui::SliderFloat("detectRadius", &sDetectRadius, 0.f, 200.f);
    GUI::End();

    // �ړ���
    Vector3 moveVec{};
    Vector3 velocity{};
    Move(moveVec, velocity); // �Q�Ɠn���Ŏ󂯎��B

    // ���W�X�V
    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos);

    // �R���C�_�[�X�V
    sphereCollider_.center = coordinate_.GetPosition();
    detectPlayerCollider_.center = coordinate_.GetPosition();

    // ���ʂ̂ǂ̈ʒu�ɂ��邩�ɉ����āA�������p���ɂ��邽�߂�3�����Čv�Z
    Vector3 rightFromOldAxis = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), coordinate_.GetForwardVec().Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �Â����ʃx�N�g��)
    coordinate_.SetAxisRight(rightFromOldAxis.Normalize());
    Vector3 forwardFromOldAxis = Math::Vec3::Cross(coordinate_.GetRightVec().Normalize(), coordinate_.GetUpVec().Normalize()); // ���ʃx�N�g���F(�X�V���ꂽ�E�x�N�g�� x �X�V���ꂽ��x�N�g��)
    coordinate_.SetAxisForward(forwardFromOldAxis.Normalize());

    // �ړ����͂��������ꍇ
    if (moveVec.IsNonZero())
    {
        // �ړ������������悤�ȁA�ړ������ɍ��킹���p���ɂ��邽�߂ɉE�����x�N�g�����Čv�Z
        Vector3 upFromAxis = coordinate_.GetUpVec(); // ��x�N�g���F(�X�V���ꂽ��x�N�g�����擾�j
        Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �ړ��x�N�g���i�ړ����� �� ���ʃx�N�g��))
        coordinate_.SetAxisRight(rightFromMoveVec.Normalize());
        coordinate_.SetAxisForward(moveVec.Normalize());
    }

    coordinate_.Update();
}

void Rabbit::Draw(void)
{
    // �ԐF�̃e�N�X�`����K�p�B�i�N�\���h���j
    appearance_->Draw("Resources/red1x1.png");
    // �f�t�H���g�\���i�Ή�����e�N�X�`�������������Ȃ��̂ŁAMissingTexture�ɒu�������B�߂�����log�o��B�j
    //appearance_->Draw(/*"Resources/red1x1.png"*/);
}

void Rabbit::Move(Vector3& moveVec, Vector3& velocity)
{
    // �ړ��x�N�g��
    //moveVec += pForwardFromCamera * inputVec.y; // ���̓x�N�g���ɉ����ĉ��Z
    //moveVec += redefinitionPRightFromCamera * inputVec.x;

    // �v���C������e�����ւ̃x�N�g�������̂܂܈ړ��x�N�g���Ƃ��ċN�p����i���j
    moveVec = (coordinate_.GetPosition() - pPos_).Normalize();

    // �d��
    jumpVecNorm_ -= kGravity_;

    // �W�����v�x�N�g��
    Vector3 jumpVec{};
    jumpVec += coordinate_.GetUpVec().Normalize() * jumpVecNorm_;

    // �ړ���
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;
}

void Rabbit::OnCollision(void)
{
    if (sphereCollider_.GetColInfo().id == "gravityArea")
    {
        // ����d�̓G���A���ɓ����Ă�ꍇ�ɍs�������B
        Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        coordinate_.SetAxisUp(center2PlayerVec.Normalize());
    }
    if (sphereCollider_.GetColInfo().id == "terrainSurface")
    {
        //isGrounded_ = true;
        //jumpVec_ = { 0,0,0 };
        jumpVecNorm_ = 0.f;

        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��H�H
        float diff = Vector3(sphereCollider_.center - sphereCollider_.GetColInfo().v).Length() - (sphereCollider_.GetColInfo().f + sphereCollider_.radius);

        Vector3 currentPos = coordinate_.GetPosition();
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // ���K�����ꂽ������v���C���[�܂ł̃x�N�g�� * �߂荞�݋���
        currentPos += coordinate_.GetUpVec().Normalize() * -diff; // �������}�C�i�X�����Œl���]

        coordinate_.SetPosition(currentPos);
    }
}

void Rabbit::OnDetectPlayer(void)
{
    if (detectPlayerCollider_.GetColInfo().id == "player")
    {
        // ���m�����v���C���̍��W���L�^����B
        pPos_ = detectPlayerCollider_.GetColInfo().v;
    }
}
