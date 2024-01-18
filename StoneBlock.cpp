#include "StoneBlock.h"

StoneBlock::StoneBlock(CollisionManager* colMPtr) : Object3D("Resources/model/cube/cube.obj"),colMPtr_(colMPtr)
{
    // マネージャーに登録
    colMPtr->Register(&collision_obb_);
    collision_obb_.SetID("stoneBlock");
    collision_obb_.callback_onCollision_ = std::bind(&StoneBlock::OnCollision, this);
    collision_obb_.radius = { 1,1,1 };

    // 初期位置
    transform_.position = { 0,60,0 };
    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
}

StoneBlock::~StoneBlock(void)
{
    // 登録を抹消
    colMPtr_->UnRegister(&collision_obb_);
}

void StoneBlock::Update(void)
{
    Object3D::Update();

    collision_obb_.center = transform_.position;
    matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);
}

void StoneBlock::Draw(void)
{
    Object3D::Draw();
}

void StoneBlock::OnCollision(void)
{
}
