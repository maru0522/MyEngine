#include "Coin.h"
#include "SimplifyImGui.h"

Coin::Coin(CollisionManager* colMPtr) : Object3D("Resources/model/coin/coin.obj"), colMPtr_(colMPtr)
{
    // コイン用コライダーの登録
    colMPtr->Register(&collision_contact_);
    collision_contact_.SetID("coin_contact");
    collision_contact_.SetOnCollision(std::bind(&Coin::Collision_Contact, this));
    collision_contact_.radius = 1.f;

    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };

    collision_contact_.center = { 3000,3000,3000 };

    se_getCoin_.SetVolume(0.3f);
}

Coin::~Coin(void)
{
    colMPtr_->UnRegister(&collision_contact_);
}

void Coin::Update(void)
{
    GUI::Begin("coin");
    GUI::Text("pos:(%f,%f,%f)", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::SliderFloat3("transform_pos", transform_.position, -200.f, 200.f);
    GUI::SliderFloat3("transform_rot", transform_.rotation, 0.f, 6.28319f);
    GUI::End();

    if (is_takenPre_ == false && is_taken_)
    {
        if (se_getCoin_.GetPlaying()) se_getCoin_.Stop();
        se_getCoin_.Play();
    }
    is_takenPre_ = is_taken_;

    transform_.rotation.y += 0.04f;
    matTrans_.mat_world = Math::Function::AffinTrans(transform_);
    Object3D::Update();

}

void Coin::Draw(void)
{
    if (is_taken_ == false) { Object3D::Draw(); }
}

void Coin::Collision_Contact(void)
{
    if (collision_contact_.GetOther()->GetID() == "player")
    {
        is_taken_ = true;
    }
}

void Coin::SetPosition(const Vector3& arg_pos)
{
    transform_.position = arg_pos;
    collision_contact_.center = arg_pos;
}
