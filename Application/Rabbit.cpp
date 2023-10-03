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
    sphereCollider_.center = { 0,60,20 };
    detectPlayerCollider_.radius = kDetectRadius_;

    // �����ʒu
    transform_.position = { 0,60,20 };
    // �����p��
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
}

void Rabbit::Update(void)
{
    if (isCaptured_ == false)
    {
        // 1Frame�x���`����W���X�V ** ���W���m�肵����ɁA�����蔻�菈���ō��W��␳���邽�߁A1Frame�x�点�Ȃ��ƃK�N���\��������B
        appearance_->GetCoordinatePtr()->mat_world = coordinate_.mat_world;
        appearance_->Update();

        static float sDetectRadius{ kDetectRadius_ };
        GUI::Begin("Rabbit");
        ImGui::SliderFloat("detectRadius", &sDetectRadius, 0.f, 200.f);
        detectPlayerCollider_.radius = sDetectRadius;
        GUI::End();

        // �ړ���
        Vector3 moveVec{};
        Vector3 velocity{};
        Move(moveVec, velocity); // �Q�Ɠn���Ŏ󂯎��B

        // ���W�X�V
        Vector3 currentPos = transform_.position;
        currentPos += velocity;
        transform_.position = currentPos;

        // �R���C�_�[�X�V
        sphereCollider_.center = currentPos;
        detectPlayerCollider_.center = currentPos;

        // ���ʂ̂ǂ̈ʒu�ɂ��邩�ɉ����āA�������p���ɂ��邽�߂�3�����Čv�Z
        Vector3 rightFromOldAxis = Math::Vec3::Cross(axes_.up.Normalize(), axes_.forward.Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �Â����ʃx�N�g��)
        axes_.right = rightFromOldAxis;
        Vector3 forwardFromOldAxis = Math::Vec3::Cross(axes_.right.Normalize(), axes_.up.Normalize()); // ���ʃx�N�g���F(�X�V���ꂽ�E�x�N�g�� x �X�V���ꂽ��x�N�g��)
        axes_.forward = forwardFromOldAxis;

        // �ړ����͂��������ꍇ
        if (moveVec.IsNonZero())
        {
            // �ړ������������悤�ȁA�ړ������ɍ��킹���p���ɂ��邽�߂ɉE�����x�N�g�����Čv�Z
            Vector3 upFromAxis = axes_.up; // ��x�N�g���F(�X�V���ꂽ��x�N�g�����擾�j
            Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �ړ��x�N�g���i�ړ����� �� ���ʃx�N�g��))
            axes_.right = rightFromMoveVec.Normalize();
            axes_.forward = moveVec.Normalize();
        }

        coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
}

void Rabbit::Draw(void)
{
    // �ԐF�̃e�N�X�`����K�p�B�i�N�\���h���j
    if (isCaptured_ == false) { appearance_->Draw("Resources/red1x1.png"); }
    // �f�t�H���g�\���i�Ή�����e�N�X�`�������������Ȃ��̂ŁAMissingTexture�ɒu�������B�߂�����log�o��B�j
    //appearance_->Draw(/*"Resources/red1x1.png"*/);
}

void Rabbit::Move(Vector3& moveVec, Vector3& velocity)
{
    // �ړ��x�N�g��
    //moveVec += pForwardFromCamera * inputVec.y; // ���̓x�N�g���ɉ����ĉ��Z
    //moveVec += redefinitionPRightFromCamera * inputVec.x;

    // �v���C������e�����ւ̃x�N�g�������̂܂܈ړ��x�N�g���Ƃ��ċN�p����i���j
    // [����]�v���C���[�̌����Ɠe�̌�������ςłƂ��āA���p�ɋ߂��قǑ��x��������x����������΁A�^�[�������Ƃ��ł��߂܂��₷���Ȃ�̂ł́H
    moveVec = (transform_.position - pPos_).Normalize();

    // �d��
    jumpVecNorm_ -= kGravity_;

    // �W�����v�x�N�g��
    Vector3 jumpVec{};
    jumpVec += axes_.up.Normalize() * jumpVecNorm_;

    // �ړ���
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;
}

void Rabbit::OnCollision(void)
{
    if (sphereCollider_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // ����d�̓G���A���ɓ����Ă�ꍇ�ɍs�������B
        Vector3 center2PlayerVec = sphereCollider_.center - other->center;
        axes_.up = center2PlayerVec.Normalize();
    }
    if (sphereCollider_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // �W�����v��
        jumpVecNorm_ = 0.f;

        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��H�H
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        Vector3 currentPos = transform_.position;
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // ���K�����ꂽ������v���C���[�܂ł̃x�N�g�� * �߂荞�݋���
        currentPos += axes_.up.Normalize() * -diff; // �������}�C�i�X�����Œl���]

        transform_.position = currentPos;
    }
    if (sphereCollider_.GetOther()->GetID() == "player")
    {
        isCaptured_ = true;
    }
}

void Rabbit::OnDetectPlayer(void)
{
    if (detectPlayerCollider_.GetOther()->GetID() == "player")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(detectPlayerCollider_.GetOther());

        // ���m�����v���C���̍��W���L�^����B
        pPos_ = other->center;
    }
}
