#include "StickyBall.h"
#include "SimplifyImGui.h"

StickyBall::StickyBall(CollisionManager* colMPtr) : Object3D("Resources/model/sphere/sphere.obj"), colMPtr_(colMPtr)
{
    // �����ʒu
    transform_.position = { 0,0,0 };
    transform_.scale = { kRadius_,kRadius_,kRadius_ };

    // �����p��
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    // �R���C�_�[�ݒ�
    colMPtr->Register(&collision_sphere_); // �}�l�[�W���[�ɓo�^
    collision_sphere_.SetID("sticky_ball"); // tag�ݒ�
    collision_sphere_.SetOnCollision(std::bind(&StickyBall::OnCollision, this)); // �Փˎ���callback�p�֐��̐ݒ�
    collision_sphere_.radius = kRadius_; // ���̏������a = �K��l
    collision_sphere_.center = transform_.position + axes_.up * kDistance_to_root_; // ���̏������W = �����̍��W + ������x�N�g�� * �K��l
}

StickyBall::~StickyBall(void)
{
    colMPtr_->UnRegister(&collision_sphere_);
}

void StickyBall::Update(void)
{
    Object3D::Update();

    // �{���z�肳��Ă��鋅�̏������W(�����̍��W + ������x�N�g�� * �K��l)
    Vector3 default_stickyPos = transform_.position + axes_.up * kDistance_to_root_;

    // ���̍��W���A1�t���[���O�̑S�̍X�V�̏I�Ձi�ՓˍX�V�������j�ŕ␳����Ă���̂ŁA����1�t���[���x��(?)
    // �z�肳�����W����ǂ̒��x����Ă��邩 = ���̌��ݍ��W - default_sticky_pos
    distance_to_defPos_ = (collision_sphere_.center - default_stickyPos).Length();
    // distance_to_defPos_��0.f�ł͂Ȃ��ꍇ����{���z�肳���ʒu�֏������␳����B ���ɏ��l�Ƃ���0.02f�܂ł̌덷�͖��������B
    float a = Math::Function::FixEpsilon<float>(distance_to_defPos_, 0.f, 0.02f);
    if (a)
    {
        // �␳���x
        const float correctSpd = 0.05f;

        // ���̌��ݍ��W���狅�̑z�菉���ւ̃x�N�g��
        Vector3 vec_curPos2DefPos = (default_stickyPos - collision_sphere_.center).Normalize();

        // ���̌��ݍ��W���擾
        Vector3 spherePos = collision_sphere_.center;
        // �擾�����ϐ��ɑ΂��ĕ␳�l�����Z
        spherePos += vec_curPos2DefPos * correctSpd;
        // �␳�ς̍��W�𔽉f
        collision_sphere_.center = spherePos;
    }

    // �����ژM��p
    GUI::Begin("sticky");
    GUI::Text("other");
    GUI::Text("def_sticPos: %f,%f,%f", default_stickyPos.x, default_stickyPos.y, default_stickyPos.z);
    GUI::Text("dist_2_defPos: %f", distance_to_defPos_);
    GUI::Text("a: %f", a);
    GUI::Space();
    GUI::Text("info at root");
    GUI::Text("pos:%f,%f,%f", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::Text("info at sphere");
    GUI::Text("pos:%f,%f,%f", collision_sphere_.center.x, collision_sphere_.center.y, collision_sphere_.center.z);
    GUI::Text("sca:%f,%f,%f", transform_.scale.x, transform_.scale.y, transform_.scale.z);
    GUI::Text("rot:%f,%f,%f", transform_.rotation.x, transform_.rotation.y, transform_.rotation.z);
    GUI::Space();
    GUI::Text("scale");
    ImGui::SliderFloat("x", &transform_.scale.x, 1.f, 10.f);
    ImGui::SliderFloat("y", &transform_.scale.y, 1.f, 10.f);
    ImGui::SliderFloat("z", &transform_.scale.z, 1.f, 10.f);
    GUI::End();

    coordinate_.mat_world = Math::Function::AffinTrans(collision_sphere_.center, transform_.scale, axes_);
}

void StickyBall::Draw(void)
{
    Object3D::Draw("Resources/jasperGreen1x1.png");
}

void StickyBall::OnCollision(void)
{
    if (collision_sphere_.GetOther()->GetID() == "player") // tag:"player"�ƐG��Ă���ꍇ
    {
        CollisionPrimitive::SphereCollider* other = static_cast<CollisionPrimitive::SphereCollider*>(collision_sphere_.GetOther());

        // �V�[��Update()��̏Փ�Update()�Ōv�Z�����
        // ���̂������o�������B���������A�����o����̋��̍��W���A(�������W + �K��l) �𒴂��Ă���ꍇ�����o������Ȃ��B

        // �v���C���[�̍��W�Ǝ��g�̋��̍��W���牟���o�����������Z�o����
        Vector3 p_to_this = (collision_sphere_.center - other->center).Normalize();

        // �߂荞�݋������o�� (�߂荞��ł���z�� - �����j���l�̓}�C�i�X�ŕԂ�
        float diff = Vector3(collision_sphere_.center - other->center).Length() - (other->radius + collision_sphere_.radius);

        // ���ݍ��W
        Vector3 currentPos = collision_sphere_.center;

        // ���K�����ꂽ������v���C���[�܂ł̃x�N�g�� * �߂荞�݋���
        currentPos += p_to_this * -diff; // �������}�C�i�X�����Œl���]

        // �ړ���̍��W���A���̍��W���K��l�ȏ㗣��Ă��Ȃ��Ȃ�A���W��␳�\
        //if ((collision_sphere_.center - currentPos).Length() < kDistance_to_defPos_)
        //{
            // ���W��␳
        collision_sphere_.center = currentPos;

        // �␳���ꂽ�l�ōs��𐶐�
        coordinate_.mat_world = Math::Function::AffinTrans(collision_sphere_.center, transform_.scale, axes_);
        //}
    }
}

void StickyBall::SetRoot(const Vector3& pos)
{
    transform_.position = pos;
    collision_sphere_.center = pos + axes_.up * kDistance_to_root_;
}
