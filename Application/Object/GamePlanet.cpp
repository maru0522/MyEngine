#include "GamePlanet.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

void GamePlanet::Initialize(CollisionManager* arg_colMPtr)
{
    IPlanet::Initialize(arg_colMPtr);

    // マネージャーに登録
    colMPtr_->Register(&repelCameraArea_);
    // 各コライダーの名前を設定
    repelCameraArea_.SetID("repelCameraArea");
    // OnCollisionの設定
    repelCameraArea_.callback_onCollision_ = std::bind(&GamePlanet::OnCollision, this);
    // 各コライダーの半径を設定
    repelCameraArea_.radius = 1.f * kRepelCameraArea_;

    Vector3 pos0 = transform_.position + Vector3{0.f, -39.f, 0.f};
    addCols_[0] = std::make_unique<TerrainSurfaceCollider>(CollisionManager::GetInstance(), pos0, 16.f);
    Vector3 pos1 = transform_.position + Vector3{0.f, 35.f, 0.f};
    addCols_[1] = std::make_unique<TerrainSurfaceCollider>(CollisionManager::GetInstance(), pos1, 18.f);
    Vector3 pos2 = transform_.position + Vector3{7.f, 2.f, -33.f};
    addCols_[2] = std::make_unique<TerrainSurfaceCollider>(CollisionManager::GetInstance(), pos2, 18.f);
}

void GamePlanet::Update(void)
{
    IPlanet::Update();

    addCols_[0]->Update();
    addCols_[1]->Update();
    addCols_[2]->Update();

#ifdef _DEBUG
    GUI::Begin("Planet");
    GUI::CheckBox("useUVChecker", &isUV_);
    GUI::End();
#endif // _DEBUG
}

void GamePlanet::Draw(void)
{
    IPlanet::Draw();

    //addCols_[0]->Draw();
    //addCols_[1]->Draw();
    //addCols_[2]->Draw();
}

void GamePlanet::Finalize(void)
{
    IPlanet::Finalize();
    colMPtr_->UnRegister(&repelCameraArea_);
}
