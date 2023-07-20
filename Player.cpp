#include "Player.h"
#include "Input.h"
#include "Quaternion.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"

Player::Player(CameraManager* camMPtr) : camMPtr_(camMPtr)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.SetOnCollision(std::bind(&Player::OnCollision, this));

    sphereCollider_.radius = kRadius_;
    coordinate_.SetPosition({ 0,60,0 }); // �����ʒu
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

void Player::Update(void)
{
    // 1Frame�x���`����W���X�V ** ���W���m�肵����ɁA�����蔻�菈���ō��W��␳���邽�߁A1Frame�x�点�Ȃ��ƃK�N���\��������B
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec().Normalize());
    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->Update();

#ifdef _DEBUG
    GUI::Begin("player", ImVec2{ 300,500 });
    GUI::Text("pos(1frame late):     [%f,%f,%f]", coordinate_.GetPosition().x, coordinate_.GetPosition().y, coordinate_.GetPosition().z);
    GUI::Text("forwardVec:           [%f,%f,%f]", coordinate_.GetForwardVec().x, coordinate_.GetForwardVec().y, coordinate_.GetForwardVec().z);
    GUI::End();
#endif // _DEBUG

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

    // 3�����Čv�Z
    Vector3 rightFromOldAxis = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), coordinate_.GetForwardVec().Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �Â����ʃx�N�g��)
    coordinate_.SetAxisRight(rightFromOldAxis.Normalize());
    Vector3 forwardFromOldAxis = Math::Vec3::Cross(coordinate_.GetRightVec().Normalize(), coordinate_.GetUpVec().Normalize()); // ���ʃx�N�g���F(�X�V���ꂽ�E�x�N�g�� x �X�V���ꂽ��x�N�g��)
    coordinate_.SetAxisForward(forwardFromOldAxis.Normalize());

    // �ړ����͂��������ꍇ
    if (moveVec.IsNonZero())
    {
        // �ړ������ɍ��킹�������Čv�Z
        Vector3 upFromAxis = coordinate_.GetUpVec(); // ��x�N�g���F(�X�V���ꂽ��x�N�g�����擾�j
        Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec.Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �ړ��x�N�g���i�ړ����� �� ���ʃx�N�g��))
        coordinate_.SetAxisRight(rightFromMoveVec.Normalize());
        coordinate_.SetAxisForward(moveVec.Normalize());
    }

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("pos(current):         [%f,%f,%f]", currentPos.x, currentPos.y, currentPos.z);
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    //GUI::Text("moveVec:              [%f,%f,%f]", moveVec.x, moveVec.y, moveVec.z);
    //GUI::Text("jumpVec:              [%f,%f,%f]", jumpVec.x, jumpVec.y, jumpVec.z);
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::Space();
    GUI::Space();
    //GUI::Text("forward(1frame late): [%f,%f,%f]", forward.x, forward.y, forward.z);
    GUI::Text("forward(current):     [%f,%f,%f]", forwardFromOldAxis.x, forwardFromOldAxis.y, forwardFromOldAxis.z);
    //GUI::Text("right(1frame late):   [%f,%f,%f]", right.x, right.y, right.z);
    GUI::Text("right(current):       [%f,%f,%f]", rightFromOldAxis.x, rightFromOldAxis.y, rightFromOldAxis.z);
    //GUI::Text("up:                   [%f,%f,%f]", upFromAxis.x, upFromAxis.y, upFromAxis.z);
    //GUI::Text("rad:                  [%f]", rad);
    //GUI::Text("inputVec:             [%f,%f]", inputVec.x, inputVec.y);
    GUI::End();
#endif // _DEBUG
}

void Player::Draw(void)
{
    // �ԐF�̃e�N�X�`����K�p�B�i�N�\���h���j
    //appearance_->Draw("Resources/red1x1.png");
    // �f�t�H���g�\���i�Ή�����e�N�X�`�������������Ȃ��̂ŁAMissingTexture�ɒu�������B�߂�����log�o��B�j
    appearance_->Draw(/*"Resources/red1x1.png"*/);
}

void Player::Move(Vector3& moveVec, Vector3& velocity)
{
    // ���̓x�N�g��
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);

    // �J�������_�̃v���C���[�ړ��x�N�g��
    Vector3 pForwardFromCamera = Math::Vec3::Cross(camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetRightVec().Normalize(), coordinate_.GetUpVec().Normalize()).Normalize(); // ����Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), pForwardFromCamera).Normalize(); // �EVec: cross(p.upVec, pForwardFromCamera)

    // �ړ��x�N�g��
    moveVec += pForwardFromCamera * inputVec.y; // ���̓x�N�g���ɉ����ĉ��Z
    moveVec += redefinitionPRightFromCamera * inputVec.x;

    // �d��
    jumpVecNorm_ -= kGravity_;

    // �W�����v�x�N�g��
    Vector3 jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += coordinate_.GetUpVec().Normalize() * jumpVecNorm_;

    // �ړ���
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("input:                [%f,%f]", inputVec.x, inputVec.y);
    GUI::Text("move:                 [%f,%f,%f]", moveVec.x, moveVec.y, moveVec.z);
    GUI::Text("pForwardFromCamera:   [%f,%f,%f]", pForwardFromCamera.x, pForwardFromCamera.y, pForwardFromCamera.z);
    //GUI::Text("pRightFromCamera:     [%f,%f,%f]", pRightFromCamera.x, pRightFromCamera.y, pRightFromCamera.z);
    GUI::Text("pRightFromCamera:     [%f,%f,%f]", redefinitionPRightFromCamera.x, redefinitionPRightFromCamera.y, redefinitionPRightFromCamera.z);
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::Space();
    GUI::Space();
    GUI::End();
#endif // _DEBUG
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
