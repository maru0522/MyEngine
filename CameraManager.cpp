#include "WorldCoordinate.h"
#include "CameraManager.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "Input.h"

using namespace Math;

Camera::Camera(const Vector3& eye) :
    eye_(eye), up_(0.f, 1.f, 0.f), rotation_(0.f, 0.f, 0.f),
    targetPtr_(nullptr), isFollow_(false),
    nearZ_(0.1f), farZ_(1000.f)
{
    UpdateOrthoGraphic();
}

void Camera::Update(void)
{
    if (KEYS::IsDown(DIK_UP)) eye_.z += 1;
    if (KEYS::IsDown(DIK_DOWN)) eye_.z -= 1;
    if (KEYS::IsDown(DIK_LEFT)) eye_.x -= 1;
    if (KEYS::IsDown(DIK_RIGHT)) eye_.x += 1;
    if (KEYS::IsDown(DIK_SPACE)) eye_.y += 1;
    if (KEYS::IsDown(DIK_LSHIFT)) eye_.y -= 1;
    WorldCoordinate coordinate;
    coordinate.SetPosition(eye_);
    coordinate.SetRotation(rotation_);
    coordinate.Update();

    isFollow_ ?
        matView_ = Math::Matrix::ViewLookAtLH(eye_, *targetPtr_, up_) :
        matView_ = Math::Matrix::ViewLookToLH(coordinate.GetPosition(), coordinate.GetAxisZ(), coordinate.GetAxisY());

    matProj_Perspective_ = Matrix::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::sWidth_, WndAPI::sHeight_, nearZ_, farZ_);
}

void Camera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Matrix::ProjectionOrthoGraphicLH(WndAPI::sWidth_, WndAPI::sHeight_);
}

CameraManager* CameraManager::GetInstance(void)
{
    static CameraManager sCameraM;
    return &sCameraM;
}

void CameraManager::Update(void)
{
    if (!current_) return;
    current_->Update();
}

void CameraManager::SetCurrentCamera(Camera* cameraPtr)
{
    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic();
    Object3D::UpdateCBMatViewPerse();
}
