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
    coordinate_.SetIsPriority(false);

    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec());
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec());
    appearance_->GetCoordinatePtr()->SetIsPriority(false);
}

void Player::Update(void)
{
    // --- 1Frame�x�� ---
    appearance_->SetCoordinate(coordinate_);
    appearance_->Update();
#ifdef _DEBUG
    GUI::Begin("player", ImVec2{ 300,500 });
    GUI::Text("pos(1frame late):     [%f,%f,%f]", coordinate_.GetPosition().x, coordinate_.GetPosition().y, coordinate_.GetPosition().z);
    GUI::End();
#endif // _DEBUG

    // ------------------
    Quaternion forward = coordinate_.GetForwardVec();
    Quaternion right = coordinate_.GetRightVec();

    // �ړ��x�N�g��
    //Vector3 moveVec{};
    //if (KEYS::IsDown(DIK_W)) moveVec += forward.ExtractVector3().Normalize();
    //if (KEYS::IsDown(DIK_S)) moveVec -= forward.ExtractVector3().Normalize();
    //if (KEYS::IsDown(DIK_A)) moveVec -= right.ExtractVector3().Normalize();
    //if (KEYS::IsDown(DIK_D)) moveVec += right.ExtractVector3().Normalize();
    Quaternion moveVec{};
    if (KEYS::IsDown(DIK_W)) moveVec += forward.Vec3Normalize();
    if (KEYS::IsDown(DIK_S)) moveVec -= forward.Vec3Normalize();
    if (KEYS::IsDown(DIK_A)) moveVec -= right.Vec3Normalize();
    if (KEYS::IsDown(DIK_D)) moveVec += right.Vec3Normalize();

    // �d��
    jumpVecNorm_ -= kGravity_;

    // �W�����v�x�N�g��
    //Vector3 jumpVec{};
    //if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    //jumpVec += coordinate_.GetUpVec().ExtractVector3().Normalize() * jumpVecNorm_;
    Quaternion jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += coordinate_.GetUpVec().ExtractVector3().Normalize() * jumpVecNorm_;

    // �ړ���
    Quaternion velocity{};
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;

    // ���W�X�V
    Quaternion currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos.ExtractVector3());

    // �R���C�_�[�X�V
    sphereCollider_.center = coordinate_.GetPosition();

    // �X�V���ꂽ���W����3�����̎����Čv�Z
    Quaternion newRight = Math::QuaternionF::CrossVector3Part(coordinate_.GetUpVec(), coordinate_.GetForwardVec()).Normalize();
    coordinate_.SetAxisRight(newRight);
    Quaternion newForward = Math::QuaternionF::CrossVector3Part(coordinate_.GetRightVec(), coordinate_.GetUpVec()).Normalize();
    coordinate_.SetAxisForward(newForward);
    Quaternion newUp = coordinate_.GetUpVec().Vec3Normalize();
    if (KEYS::IsDown(DIK_RIGHT))
    {
        newUp.w += 0.1f;
    }
    coordinate_.SetAxisUp(newUp);

#ifdef _DEBUG
    GUI::Begin("player");
    GUI::Text("pos(current):         [%f,%f,%f]", currentPos.x, currentPos.y, currentPos.z);
    GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::Text("moveVec:              [%f,%f,%f]", moveVec.x, moveVec.y, moveVec.z);
    GUI::Text("jumpVec:              [%f,%f,%f]", jumpVec.x, jumpVec.y, jumpVec.z);
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::Space();
    GUI::Space();
    GUI::Text("forward(1frame late): [%f,%f,%f]", forward.x, forward.y, forward.z);
    GUI::Text("forward(current):     [%f,%f,%f]", newForward.x, newForward.y, newForward.z);
    GUI::Text("right(1frame late):   [%f,%f,%f]", right.x, right.y, right.z);
    GUI::Text("right(current):       [%f,%f,%f]", newRight.x, newRight.y, newRight.z);
    GUI::Text("up:                   [%f,%f,%f]", newUp.x, newUp.y, newUp.z);
    GUI::Text("up:                   [%f]", newUp.w);
    GUI::End();
#endif // _DEBUG
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
        coordinate_.SetAxisUp({ center2PlayerVec.Normalize(), coordinate_.GetUpVec().w});
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
        currentPos += coordinate_.GetUpVec().ExtractVector3().Normalize() * -diff; // �������}�C�i�X�����Œl���]

        coordinate_.SetPosition(currentPos);
    }
}
