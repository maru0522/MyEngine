#include "Rock.h"
#include "MathUtil.h"

Rock::Rock(CollisionManager* colMPtr) : Object3D("Resources/model/cube/cube.obj"), colMPtr_(colMPtr)
{
    colMPtr->Register(&collision_aabb_);
    collision_aabb_.SetID("rock");
    collision_aabb_.SetOnCollision(std::bind(&Rock::OnCollision, this));
    collision_aabb_.radius = kRadius_;
    transform_.scale = kRadius_; // cube‚Ìxyz‚ª‘S‚Ä1‚¾‚©‚ç‰Â”\‚È•¶

    // ‰ŠúˆÊ’u
    transform_.position = { 0,60,0 };
    // ‰ŠúŽp¨
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
}

Rock::~Rock(void)
{
    colMPtr_->UnRegister(&collision_aabb_);
}

void Rock::Update(void)
{
    Object3D::Update();

    collision_aabb_.center = transform_.position;
    coordinate_.mat_world = Math::Function::AffinTrans(transform_,axes_);
}

void Rock::Draw(void)
{
    Object3D::Draw();
}

void Rock::OnCollision(void)
{
}
