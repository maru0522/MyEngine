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
    static Quaternion oldForward = coordinate_.GetForwardVec();
    static Quaternion oldRight = coordinate_.GetRightVec();
    static Quaternion oldUp = coordinate_.GetUpVec();

    // --- 1Frame�x�� ---

    //// 1frame�O�ƌ���frame�̉E�x�N�g���ł̊p�x�����߂�
    //float theta4Forward = std::acosf(Math::Vec3::Dot(oldRight.ExtractVector3().Normalize(), coordinate_.GetRightVec().ExtractVector3().Normalize()));
    //// ���̐��ʃx�N�g���Ɗp�x����A���ʃx�N�g��(roll)�̔C�ӎ���]�N�H�[�^�j�I�������B
    //Quaternion forward4Appearance = Math::QuaternionF::MakeAxisAngle(coordinate_.GetForwardVec().ExtractVector3().Normalize(), theta4Forward);

    //// 1frame�O�ƌ���frame�̏�x�N�g���ł̊p�x�����߂�
    //float theta4Right = std::acosf(Math::Vec3::Dot(oldUp.ExtractVector3().Normalize(), coordinate_.GetUpVec().ExtractVector3().Normalize()));
    //// ���̉E�x�N�g���Ɗp�x����A�E�x�N�g��(pitch)�̔C�ӎ���]�N�H�[�^�j�I�������B
    //Quaternion right4Appearance = Math::QuaternionF::MakeAxisAngle(coordinate_.GetRightVec().ExtractVector3().Normalize(), theta4Right);

    //// 1frame�O�ƌ���frame�̐��ʃx�N�g���Ŋp�x�����߂�
    //float theta4Up = std::acosf(Math::Vec3::Dot(oldForward.ExtractVector3().Normalize(), coordinate_.GetForwardVec().ExtractVector3().Normalize()));
    //// ���̏�x�N�g���Ɗp�x����A��x�N�g��(yaw)�̔C�ӎ���]�N�H�[�^�j�I�������B
    //Quaternion up4Appearance = Math::QuaternionF::MakeAxisAngle(coordinate_.GetUpVec().ExtractVector3().Normalize(), theta4Up);

    // �O���̃N�H�[�^�j�I�����Z�b�g
    //appearance_->GetCoordinatePtr()->SetAxis({ forward4Appearance, right4Appearance, up4Appearance });
    // �X�V���ꂽ���W���Z�b�g�i1frame�x��)
    if (KEYS::IsTrigger(DIK_K))
    {
        appearance_->GetCoordinatePtr()->SetAxisUp({ 0,1,0 });
        appearance_->GetCoordinatePtr()->SetAxisForward({ 0,0,1 });
        appearance_->GetCoordinatePtr()->SetAxisRight({ 1,0,0 });
    }
    if (KEYS::IsTrigger(DIK_O))
    {
        // �i���ʃx�N�g��)roll������-90�x��]������ԂɂȂ�Ǝv���񂾂��ǂȁ[
        //appearance_->GetCoordinatePtr()->SetAxisUp(Vector3{ 1,4,1 }.Normalize());
        //appearance_->GetCoordinatePtr()->SetAxisForward(Vector3{ 3,0,1 }.Normalize());
        //appearance_->GetCoordinatePtr()->SetAxisRight(Vector3{ 0,-1,5 }.Normalize());
        appearance_->GetCoordinatePtr()->SetAxisUp(Vector3{ 1,0,0 }.Normalize());
        appearance_->GetCoordinatePtr()->SetAxisForward(Vector3{ 0,0,1 }.Normalize());
        appearance_->GetCoordinatePtr()->SetAxisRight(Vector3{ 0,-1,0 }.Normalize());
    }
    appearance_->GetCoordinatePtr()->SetAxisUp(coordinate_.GetUpVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisForward(coordinate_.GetForwardVec().Normalize());
    appearance_->GetCoordinatePtr()->SetAxisRight(coordinate_.GetRightVec().Normalize());
    appearance_->GetCoordinatePtr()->SetPosition(coordinate_.GetPosition());
    appearance_->Update();

#ifdef _DEBUG
    GUI::Begin("player", ImVec2{ 300,500 });
    GUI::Text("pos(1frame late):     [%f,%f,%f]", coordinate_.GetPosition().x, coordinate_.GetPosition().y, coordinate_.GetPosition().z);
    GUI::End();
#endif // _DEBUG
    // ------------------

    // �ړ���
    Vector3 velocity = Move();

    // ���W�X�V
    Vector3 currentPos = coordinate_.GetPosition();
    currentPos += velocity;
    coordinate_.SetPosition(currentPos);

    // �R���C�_�[�X�V
    sphereCollider_.center = coordinate_.GetPosition();

    // �X�V���O�̊e���x�N�g���̕ۑ�
    oldForward = coordinate_.GetForwardVec();
    oldRight = coordinate_.GetRightVec();
    oldUp = coordinate_.GetUpVec();

    // �X�V���ꂽ���W����3�����̎����Čv�Z
    Vector3 newRight = Math::Vec3::Cross(coordinate_.GetUpVec().Normalize(), coordinate_.GetForwardVec().Normalize()).Normalize();
    coordinate_.SetAxisRight(newRight);
    Vector3 newForward = Math::Vec3::Cross(coordinate_.GetRightVec().Normalize(), coordinate_.GetUpVec().Normalize());
    coordinate_.SetAxisForward(newForward);
    Vector3 newUp = coordinate_.GetUpVec().Normalize();

    coordinate_.SetAxisUp(newUp);

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
    GUI::Text("forward(current):     [%f,%f,%f]", newForward.x, newForward.y, newForward.z);
    //GUI::Text("right(1frame late):   [%f,%f,%f]", right.x, right.y, right.z);
    GUI::Text("right(current):       [%f,%f,%f]", newRight.x, newRight.y, newRight.z);
    GUI::Text("up:                   [%f,%f,%f]", newUp.x, newUp.y, newUp.z);
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

Vector3 Player::Move(void)
{
    Vector3 forward = coordinate_.GetForwardVec().Normalize();
    Vector3 right = coordinate_.GetRightVec().Normalize();

    // �ړ��x�N�g��
    Vector3 moveVec{};
    if (KEYS::IsDown(DIK_W)) moveVec += forward;
    if (KEYS::IsDown(DIK_S)) moveVec -= forward;
    if (KEYS::IsDown(DIK_A)) moveVec -= right;
    if (KEYS::IsDown(DIK_D)) moveVec += right;

    // �d��
    jumpVecNorm_ -= kGravity_;

    // �W�����v�x�N�g��
    Vector3 jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += coordinate_.GetUpVec().Normalize() * jumpVecNorm_;

    // �ړ���
    Vector3 velocity{};
    velocity += moveVec.Normalize() * kMoveSpeed_;
    velocity += jumpVec;

    return velocity;
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
