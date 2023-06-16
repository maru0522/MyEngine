#include "WorldCoordinate.h"
#include "CameraManager.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "Input.h"

using namespace Math;
using namespace Input;

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

    // debugCamera
    if (isDebugMode_) {
        const Vector2& mouseVelocity = Mouse::GetCursorVec();

        // ��]
        if (Mouse::IsDown(Mouse::Click::RIGHT)) { // �E�N�������Ă�
            const float rotSpeed = 0.001f;
            rotation_.x += Mouse::GetCursorVec().y * rotSpeed;
            rotation_.y += Mouse::GetCursorVec().x * rotSpeed;
        }

        // ���s�ړ�
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && Mouse::IsDown(Mouse::Click::CENTER)) { // �E�N�������ĂȂ� && �z�C�[�������Ă�
            const float moveSpeed = 0.05f;
            eye_ += coordinate.GetAxisX().normalize() * mouseVelocity.x * moveSpeed;
            eye_ += coordinate.GetAxisY().normalize() * -mouseVelocity.y * moveSpeed;
        }

        // �O��ړ�
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && !Mouse::IsDown(Mouse::Click::CENTER)) { // �E�N�������ĂȂ� && �z�C�[�������ĂȂ�
            const float moveSpeed = 0.01f;
            eye_ += coordinate.GetAxisZ().normalize() * Mouse::GetScroll() * moveSpeed;
        }
    }

    isFollow_ ?
        matView_ = Math::Matrix::ViewLookAtLH(eye_, *targetPtr_, up_) :
        matView_ = Math::Matrix::ViewLookToLH(coordinate.GetPosition(), coordinate.GetAxisZ(), coordinate.GetAxisY());

    matProj_Perspective_ = Matrix::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}

void Camera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Matrix::ProjectionOrthoGraphicLH(WndAPI::kWidth_, WndAPI::kHeight_);
}

void Camera::Follow(Vector3* p_target)
{
    isFollow_ = true;
    targetPtr_ = p_target;
}

void Camera::UnFollow(void)
{
    isFollow_ = false;
    targetPtr_ = nullptr;
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
    // �g�p�̗L���Ɋ֌W�Ȃ��f�o�b�O�J�������[�h��OFF
    current_->SetIsDebugMode(false);

    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic();
    Object3D::UpdateCBMatViewPerse();
}
