#include "Player.h"
#include "Input.h"
#include "Quaternion.h"
#include "SimplifyImGui.h"
#include "CollisionChecker.h"
#include "CollisionManager.h"

Player::Player(CameraManager* camMPtr) : camMPtr_(camMPtr),pbm_(this,PlayerBehavior::IDLE)
{
    CollisionManager::GetInstance()->Register(&sphereCollider_);
    sphereCollider_.SetID("player");
    sphereCollider_.SetOnCollision(std::bind(&Player::OnCollision, this));

    sphereCollider_.radius = kRadius_;

    // �����ʒu
    transform_.position = { 0,60,-10 };
    // �����p��
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    moveVec_ = { 0,1,0 };
}

Player::~Player(void)
{
    CollisionManager::GetInstance()->UnRegister(&sphereCollider_);
}

void Player::Update(void)
{
    if (isFallHole1_)
    {
        transform_.position = { 0,-15,48 };
        isFallHole1_ = false;
    }
    if (isFallHole2_)
    {
        transform_.position = { 0,15,-48 };
        isFallHole2_ = false;
    }

    // 1Frame�x���`����W���X�V ** ���W���m�肵����ɁA�����蔻�菈���ō��W��␳���邽�߁A1Frame�x�点�Ȃ��ƃK�N���\��������B
    appearance_->GetCoordinatePtr()->mat_world = coordinate_.mat_world;
    appearance_->Update();

    pbm_.ManagementBehavior();

    // �R���C�_�[�X�V
    sphereCollider_.center = transform_.position;

    //// ���ʂ̂ǂ̈ʒu�ɂ��邩�ɉ����āA�������p���ɂ��邽�߂�3�����Čv�Z
    //Vector3 rightFromOldAxis = Math::Vec3::Cross(axes_.up, axes_.forward); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �Â����ʃx�N�g��)
    //axes_.right = rightFromOldAxis.Normalize();
    //Vector3 forwardFromOldAxis = Math::Vec3::Cross(axes_.right, axes_.up); // ���ʃx�N�g���F(�X�V���ꂽ�E�x�N�g�� x �X�V���ꂽ��x�N�g��)
    //axes_.forward = forwardFromOldAxis.Normalize();

    //// �ړ����͂��������ꍇ
    //if (moveVec_.IsNonZero())
    //{
    //    // �ړ������������悤�ȁA�ړ������ɍ��킹���p���ɂ��邽�߂ɉE�����x�N�g�����Čv�Z
    //    Vector3 upFromAxis = axes_.up; // ��x�N�g���F(�X�V���ꂽ��x�N�g�����擾�j
    //    Vector3 rightFromMoveVec = Math::Vec3::Cross(upFromAxis.Normalize(), moveVec_.Normalize()); // �E�x�N�g���F(�X�V���ꂽ��x�N�g�� x �ړ��x�N�g���i�ړ����� �� ���ʃx�N�g��))
    //    axes_.right = rightFromMoveVec.Normalize();
    //    axes_.forward = moveVec_.Normalize();
    //}

    // ���݂̍��W�ōs��𐶐��i�d�͂ɂ���Ă߂荞��ł���B�j�@-> �߂荞�ݕ␳��OnCollision()�Ɉ��p��
    // �v�Z�ʂ����炵�����ꍇ�A�R�~�b�gID a02ba1f80360bda078a7dbb7ea2e8447064e6e9d ���Q��
    coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);

#ifdef _DEBUG
    GUI::Begin("Control");
    GUI::Text("Move:WASD");
    GUI::Text("Jump:SPACE");
    GUI::End();

    GUI::Begin("player");

    switch (pbm_.GetStatePtr()->GetCurState())
    {
    case PlayerBehavior::IDLE:
        GUI::Text("Behavior: Idle");
        break;
    case PlayerBehavior::MOVE:
        GUI::Text("Behavior: Move");
        break;
    case PlayerBehavior::STOOP:
        GUI::Text("Behavior: STOOP");
        break;
    case PlayerBehavior::MOVE_STOOP:
        GUI::Text("Behavior: MOVE_STOOP");
        break;
    case PlayerBehavior::JUMP:
        GUI::Text("Behavior: JUMP");
        break;
    default:
        GUI::Text("Behavior: Unknown");
        break;
    }
    //GUI::Text("pos(current):         [%f,%f,%f]", currentPos.x, currentPos.y, currentPos.z);
    //GUI::Text("velocity:             [%f,%f,%f]", velocity.x, velocity.y, velocity.z);
    GUI::Text("jumpVecNorm:          [%f]", jumpVecNorm_);
    GUI::Text("kGravity:             [%f]", kGravity_);
    GUI::Space();
    GUI::Space();
    //GUI::Text("forward(current):     [%f,%f,%f]", forwardFromOldAxis.x, forwardFromOldAxis.y, forwardFromOldAxis.z);
    //GUI::Text("right(current):       [%f,%f,%f]", rightFromOldAxis.x, rightFromOldAxis.y, rightFromOldAxis.z);

    ImGui::Text("matrix");
    Matrix4 p = coordinate_.mat_world;
    ImGui::Text("%f, %f, %f, %f", p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3]);
    ImGui::Text("%f, %f, %f, %f", p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);
    GUI::Text("pos(current):         [%f,%f,%f]", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::Text("rot(current):         [%f,%f,%f]", transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
    GUI::Text("sca(current):         [%f,%f,%f]", transform_.scale.x, transform_.scale.y, transform_.scale.z);
    GUI::Space();
    GUI::Text("forward(current):     [%f,%f,%f]", axes_.forward.x, axes_.forward.y, axes_.forward.z);
    GUI::Text("right(current):       [%f,%f,%f]", axes_.right.x, axes_.right.y, axes_.right.z);
    GUI::Text("up(current):          [%f,%f,%f]", axes_.up.x, axes_.up.y, axes_.up.z);
    GUI::End();
#endif // _DEBUG
}

void Player::Draw(void)
{
    // �ԐF�̃e�N�X�`����K�p�B�i�N�\���h���j
    //appearance_->Draw("Resources/red1x1.png");
    // �f�t�H���g�\���i�Ή�����e�N�X�`�������������Ȃ��̂ŁAMissingTexture�ɒu�������B�߂�����log�o��B�j
    if (pbm_.GetStatePtr()->GetCurState() == PlayerBehavior::STOOP || pbm_.GetStatePtr()->GetCurState() == PlayerBehavior::MOVE_STOOP) { appearance_->Draw("Resources/red1x1.png"); }
    else { appearance_->Draw(/*"Resources/red1x1.png"*/); }
}

void Player::Move(Vector3& moveVec, Vector3& velocity)
{
    // ���̓x�N�g��
    Vector3 inputVec{};
    inputVec.x = (float)KEYS::IsDown(DIK_D) - KEYS::IsDown(DIK_A);
    inputVec.y = (float)KEYS::IsDown(DIK_W) - KEYS::IsDown(DIK_S);
    inputVec = inputVec.Normalize();

    // �J�������_�̃v���C���[�ړ��x�N�g��
    Vector3 pForwardFromCamera = Math::Vec3::Cross(camMPtr_->GetCurrentCamera()->GetAxis3Ptr()->right, axes_.up).Normalize(); // ����Vec: cross(camera.rightVec, p.upVec)
    Vector3 redefinitionPRightFromCamera = Math::Vec3::Cross(axes_.up, pForwardFromCamera).Normalize(); // �EVec: cross(p.upVec, pForwardFromCamera)

    // �ړ��x�N�g��
    moveVec += pForwardFromCamera * inputVec.y; // ���̓x�N�g���ɉ����ĉ��Z
    moveVec += redefinitionPRightFromCamera * inputVec.x;

    // �J�������W�p�̒l��␳
    SphericalCamera(inputVec);

    // �d��
    jumpVecNorm_ -= kGravity_;

    // �W�����v�x�N�g��
    Vector3 jumpVec{};
    if (KEYS::IsTrigger(DIK_SPACE)) { jumpVecNorm_ = kJumpPower_; }
    jumpVec += axes_.up * jumpVecNorm_;

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

void Player::SphericalCamera(Vector3& inputVec)
{
    if (jumpVecNorm_)
    {
        // �J�����ƃv���C���[�̋���
        float dist = (camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - transform_.position).Length();

        // �W�����v���ɃJ�����̒Ǐ]���y�� �� ��ʂ̗h���}������ړI
        // ���ς��K��l�����̎��W�����v���J��Ԃ��ƃJ���������ǂ�ǂ񉓂��Ȃ��Ă����s����o�Ă�
        current_rad_ = dist;
    }

    // �v���C���[�̐��ʂƃJ�����̐��ʂ̓��ς� "�K��l" �����̎�
    // �K��l�̒l������������قǁA�v���C���[����ʒ����ɋ߂��ʒu�ŁA�J�����̋������؂�ւ��B
    if (axes_.forward.Dot(camMPtr_->GetCurrentCamera()->GetAxis3Ptr()->forward) < 0.7f)
    {
        // �J�����ƃv���C���[�̋���
        float dist = (camMPtr_->GetCurrentCamera()->GetCoordinatePtr()->GetMatPos() - transform_.position).Length();

        // �Y���������A�{���ݒ肳��Ă���v���C���[�Ƃ̋������Z���ꍇ�A�Y��������ݒ苗���Ƃ���B
        if (dist < current_rad_)
        {
            // �v���C���[���J�������Ɍ������Ĉړ�����ہA�J�����̍��W���Œ肷��Ӑ}
            // �������A���󂾂ƃJ�������������鏈������肭�@�\���Ă��Ȃ��׃R�����g�A�E�g�B
            //current_rad_ = dist;
        }
    }
    else
    {
        // ���݋���(cureent_rad_)���A��������(default_rad_)��菬�����l�Ȃ�A���݋�����␳����B
        if (current_rad_ < default_rad_)
        {
            current_rad_ += 0.1f;
            //current_rad_ = Math::Ease::EaseInSine(current_rad_, current_rad_, default_rad_);
        }
        else if (current_rad_ > default_rad_)
        {
            current_rad_ -= 0.1f;
        }

        theta_ += 0.02f * inputVec.y;
        phi_ += 0.02f * inputVec.x;
        Math::Function::Loop(theta_, 0.f, 6.28319f);
        Math::Function::Loop(phi_, 0.f, 6.28319f);
    }
}

void Player::OnCollision(void)
{
    if (sphereCollider_.GetOther()->GetID() == "gravityArea")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // �{���͋���d�̓G���A���ɓ����Ă�ꍇ�ɍs�������B
        //Vector3 center2PlayerVec = sphereCollider_.center - sphereCollider_.GetColInfo().v;
        Vector3 center2PlayerVec = sphereCollider_.center - other->center;
        axes_.up = center2PlayerVec.Normalize();
    }
    if (sphereCollider_.GetOther()->GetID() == "terrainSurface")
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(sphereCollider_.GetOther());

        // �W�����v�����
        jumpVecNorm_ = 0.f;

        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j�Ȃ̂Ō��ʂ̓}�C�i�X�z��H�H
        float diff = Vector3(sphereCollider_.center - other->center).Length() - (other->radius + sphereCollider_.radius);

        Vector3 currentPos = transform_.position;
        //currentPos += player->body_->coordinate_.GetUpVec().ExtractVector3();

        // ���K�����ꂽ������v���C���[�܂ł̃x�N�g�� * �߂荞�݋���
        currentPos += axes_.up * -diff; // �������}�C�i�X�����Œl���]

        // ���W��␳
        transform_.position = currentPos;

        // �␳���ꂽ�l�ōs��𐶐�
        coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rock")
    {
        // �߂荞�݋����̎Z�o���@���������BAABB���̔��a�ǂ�����ĎZ�o������B

        // ���ݍ��W
        Vector3 currentPos = transform_.position;

        // �ړ����������������߂��悤�ɂ���B
        currentPos -= velocity_;

        // ���W��␳
        transform_.position = currentPos;

        // �␳���ꂽ�l�ōs��𐶐�
        coordinate_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    }
    if (sphereCollider_.GetOther()->GetID() == "rabbit")
    {
        captureCount_rabbit++;
    }
    if (sphereCollider_.GetOther()->GetID() == "tunnel1")
    {
        isFallHole1_ = true;
    }
    if (sphereCollider_.GetOther()->GetID() == "tunnel2")
    {
        isFallHole2_ = true;
    }
}