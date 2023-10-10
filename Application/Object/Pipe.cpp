#include "Pipe.h"
#include "MathUtil.h"

Pipe::Pipe(CollisionManager* colMPtr) : Object3D("Resources/model/pipe/pipe.obj"),colMPtr_(colMPtr)
{
    // 押し戻し用コライダーの登録
    colMPtr->Register(&collision_pushback_);
    collision_pushback_.SetID("pipe_pushback");
    collision_pushback_.SetOnCollision(std::bind(&Pipe::Collision_PushBack, this));
    // 土管に入る時用のコライダーの登録
    colMPtr->Register(&collision_enterInside_);
    collision_enterInside_.SetID("pipe_enterInside");
    collision_enterInside_.SetOnCollision(std::bind(&Pipe::Collision_EnterInside, this));
}

Pipe::~Pipe(void)
{
    // 押し戻し用コライダーの抹消
    colMPtr_->UnRegister(&collision_pushback_);
    // 土管に入る時用のコライダーの抹消
    colMPtr_->UnRegister(&collision_enterInside_);
}

void Pipe::Update(void)
{
    matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    Object3D::Update();
}

void Pipe::Draw(void)
{
    Object3D::Draw();
}

void Pipe::Collision_PushBack(void)
{
}

void Pipe::Collision_EnterInside(void)
{
}

void Pipe::SetPosition(const Vector3& arg_pos)
{
    transform_.position = arg_pos;
    collision_pushback_.center = arg_pos;
    collision_enterInside_.center = arg_pos;
}
