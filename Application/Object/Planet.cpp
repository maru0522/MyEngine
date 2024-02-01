#include "Planet.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

void Planet::Initialize(CollisionManager* arg_colMPtr)
{
    colMPtr_ = arg_colMPtr;

    // マネージャーに登録
    colMPtr_->Register(&surface_);
    colMPtr_->Register(&gravityArea_);
    colMPtr_->Register(&repelCameraArea_);

    // 各コライダーの名前を設定
    surface_.SetID("terrainSurface");
    gravityArea_.SetID("gravityArea");
    repelCameraArea_.SetID("repelCameraArea");

    // OnCollisionの設定
    surface_.callback_onCollision_ = std::bind(&Planet::OnCollision, this);
    gravityArea_.callback_onCollision_ = std::bind(&Planet::OnCollision, this);
    repelCameraArea_.callback_onCollision_ = std::bind(&Planet::OnCollision, this);

    // 星自体の座標とスケールの設定
    transform_.position = { 0,0,0 };
    transform_.scale = { kScale_,kScale_,kScale_ };

    // 各コライダーの半径を設定
    surface_.radius = 1.f * kScale_;
    gravityArea_.radius = 1.f * kGravityArea_;
    repelCameraArea_.radius = 1.f * kRepelCameraArea_;

    // 丸影が投影される
    appearance_->SetIsShadowFlash(true);

    Vector3 pos0 = transform_.position + Vector3{0.f, -39.f, 0.f};
    addCols_[0] = std::make_unique<TerrainSurfaceCollider>(CollisionManager::GetInstance(), pos0, 16.f);
    Vector3 pos1 = transform_.position + Vector3{0.f, 35.f, 0.f};
    addCols_[1] = std::make_unique<TerrainSurfaceCollider>(CollisionManager::GetInstance(), pos1, 18.f);
    Vector3 pos2 = transform_.position + Vector3{7.f, 2.f, -33.f};
    addCols_[2] = std::make_unique<TerrainSurfaceCollider>(CollisionManager::GetInstance(), pos2, 18.f);
}

void Planet::Update(void)
{
    appearance_->GetCoordinatePtr()->mat_world = Math::Function::AffinTrans(transform_);
    appearance_->Update();

    surface_.center = transform_.position;

    addCols_[0]->Update();
    addCols_[1]->Update();
    addCols_[2]->Update();

#ifdef _DEBUG
    GUI::Begin("Planet");
    GUI::CheckBox("useUVChecker", &isUV_);
    GUI::End();
#endif // _DEBUG
}

void Planet::Draw(void)
{
    isUV_ ?
        appearance_->Draw("Resources/uvchecker.jpg") :
        appearance_->Draw("Resources/planet4.png");

    //addCols_[0]->Draw();
    //addCols_[1]->Draw();
    //addCols_[2]->Draw();
}

void Planet::Finalize(void)
{
    colMPtr_->UnRegister(&surface_);
    colMPtr_->UnRegister(&gravityArea_);
    colMPtr_->UnRegister(&repelCameraArea_);
}
