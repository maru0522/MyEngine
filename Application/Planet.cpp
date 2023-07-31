#include "Planet.h"
#include "CollisionManager.h"
#include "SimplifyImGui.h"

Planet::Planet(void)
{
    // �}�l�[�W���[�ɓo�^
    CollisionManager::GetInstance()->Register(&surface_);
    CollisionManager::GetInstance()->Register(&gravityArea_);
    CollisionManager::GetInstance()->Register(&repelCameraArea_);

    // �e�R���C�_�[�̖��O��ݒ�
    surface_.SetID("terrainSurface");
    gravityArea_.SetID("gravityArea");
    repelCameraArea_.SetID("repelCameraArea");

    // OnCollision�̐ݒ�
    surface_.SetOnCollision(std::bind(&Planet::OnCollision, this));
    gravityArea_.SetOnCollision(std::bind(&Planet::OnCollision, this));
    repelCameraArea_.SetOnCollision(std::bind(&Planet::OnCollision, this));

    // �����̂̍��W�ƃX�P�[���̐ݒ�
    body_->GetCoordinatePtr()->SetPosition({ 0,0,0 });
    body_->GetCoordinatePtr()->SetScale({ kScale_,kScale_,kScale_ });

    // �e�R���C�_�[�̔��a��ݒ�
    surface_.radius = 1.f * kScale_;
    gravityArea_.radius = 1.f * kGravityArea_;
    repelCameraArea_.radius = 1.f * kRepelCameraArea_;
}

void Planet::Update(void)
{
    body_->Update();

    surface_.center = body_->GetCoordinatePtr()->GetPosition();
}

void Planet::Draw(void)
{
    body_->Draw();
}
