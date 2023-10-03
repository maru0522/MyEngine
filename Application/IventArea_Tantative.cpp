#include "IventArea_Tantative.h"

IventArea_Tantative::IventArea_Tantative(CollisionManager* colMPtr, const std::string& id) : Object3D("Resources/model/cube/cube.obj"),colMPtr_(colMPtr)
{
    colMPtr->Register(&aabb_);
    aabb_.SetID(id);
    aabb_.SetOnCollision(std::bind(&IventArea_Tantative::OnCollision, this));
    aabb_.radius = kRadius_;

    transform_.position = { 0, 0, 0 };
    transform_.scale = kRadius_; // cube‚Ìxyz‚ª‘S‚Ä1‚¾‚©‚ç‰Â”\‚È•¶
    transform_.rotation = { 0, 0, 0 };

    // ‰ŠúŽp¨
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
}

IventArea_Tantative::~IventArea_Tantative(void)
{
    colMPtr_->UnRegister(&aabb_);
}

void IventArea_Tantative::Update(void)
{
    Object3D::Update();

    aabb_.center = transform_.position;
    coordinate_.mat_world = Math::Function::AffinTrans(transform_.position, transform_.scale, transform_.rotation);
}

void IventArea_Tantative::Draw(void)
{
    Object3D::Draw("Resources/red1x1.png");
}

void IventArea_Tantative::OnCollision(void)
{
}
