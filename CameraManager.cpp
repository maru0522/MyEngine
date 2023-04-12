#include "CameraManager.h"
Camera* CameraManager::current_{ nullptr };

Camera::Camera(void) :
    eye_({ 0.f,0.f,0.f }), target_({ 0.f,0.f,0.f }), up_({ 0.f,1.f,0.f }),
    nearZ_(0.1f), farZ_(1000.f),
    state_(CameraProjection::ORTHOGRAPHIC)
{
}

Camera::Camera(const Vector3& eye, const Vector3& target, const Vector3& up) :
    eye_(eye), target_(target), up_(up),
    nearZ_(0.1f), farZ_(1000.f),
    state_(CameraProjection::ORTHOGRAPHIC)
{
}

void Camera::SwitchState(void)
{
    state_ == CameraProjection::ORTHOGRAPHIC ?
        state_ = CameraProjection::PERSPECTIVE :
        state_ = CameraProjection::ORTHOGRAPHIC;
}

void CameraManager::Update(void)
{
}
