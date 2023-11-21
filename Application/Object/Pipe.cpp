#include "Pipe.h"
#include "MathUtil.h"
#include "SimplifyImGui.h"

Pipe::Pipe(CollisionManager* colMPtr) : Object3D("Resources/model/pipe/pipe.obj"),colMPtr_(colMPtr)
{
    // 押し戻し用コライダーの登録
    colMPtr->Register(&collision_pushback_);
    collision_pushback_.SetID("pipe_pushback");
    collision_pushback_.callback_onCollision_ = std::bind(&Pipe::Collision_PushBack, this);
    collision_pushback_.radius = 2.8f;
    // 土管に入る時用のコライダーの登録
    colMPtr->Register(&collision_enterInside_);
    collision_enterInside_.SetID("pipe_enterInside");
    collision_enterInside_.callback_onCollision_ = std::bind(&Pipe::Collision_EnterInside, this);
    collision_enterInside_.radius = 1.f;

    // スケール設定
    transform_.scale = { 1.8f,1.8f,1.8f };

    // 初期姿勢
    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
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
    GUI::Begin("pipe");
    GUI::Text("pos:(%f,%f,%f)", transform_.position.x, transform_.position.y, transform_.position.z);
    GUI::SliderFloat3("transform_pos", transform_.position, -200.f, 200.f);
    GUI::SliderFloat3("transform_rot", transform_.rotation, 0.f, 6.28319f);
    //GUI::SliderFloat3("axis_forward", axes_.forward, -1.f, 1.f);
    //GUI::SliderFloat3("axis_right", axes_.right, -1.f, 1.f);
    //GUI::SliderFloat3("axis_up", axes_.up, -1.f, 1.f);
    GUI::End();
    //matTrans_.mat_world = Math::Function::AffinTrans(transform_, axes_);
    matTrans_.mat_world = Math::Function::AffinTrans(transform_);
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
