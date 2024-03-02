#include "IPlanet.h"

void IPlanet::Initialize(CollisionManager* arg_colMPtr)
{
    appearance_ = std::make_unique<Object3D>(modelPath_);

    colMPtr_ = arg_colMPtr;

    // マネージャーに登録
    colMPtr_->Register(&surface_);
    colMPtr_->Register(&gravityArea_);

    // 各コライダーの名前を設定
    surface_.SetID("terrainSurface");
    gravityArea_.SetID("gravityArea");

    // OnCollisionの設定
    surface_.callback_onCollision_ = std::bind(&IPlanet::OnCollision, this);
    gravityArea_.callback_onCollision_ = std::bind(&IPlanet::OnCollision, this);

    // 星自体の座標とスケールの設定
    transform_.position = { 0,0,0 };
    transform_.scale = { kScale_,kScale_,kScale_ };

    // 各コライダーの半径を設定
    surface_.radius = 1.f * kScale_;
    gravityArea_.radius = 1.f * kGravityArea_;

    // 丸影が投影される
    appearance_->SetIsShadowFlash(true);
}

void IPlanet::Update(void)
{
    appearance_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(transform_);
    appearance_->Update();

    surface_.center = transform_.position;
}

void IPlanet::Draw(void)
{
    isUV_ ?
        appearance_->Draw("Resources/uvchecker.jpg") :
        appearance_->Draw("Resources/planet.png");
}

void IPlanet::Finalize(void)
{
    colMPtr_->UnRegister(&surface_);
    colMPtr_->UnRegister(&gravityArea_);
}
