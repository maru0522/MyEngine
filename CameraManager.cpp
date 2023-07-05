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
    nearZ_(0.1f), farZ_(1000.f),
    eyeDirection_(0.f, 0.f, 0.f)
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

    coordinate_.Reset();
    coordinate_.SetPosition(eye_);
    coordinate_.SetRotation(rotation_);
    coordinate_.Update();

    // debugCamera
    if (isDebugMode_) {
        const Vector2& mouseVelocity = Mouse::GetCursorVec();

        // ��]
        if (Mouse::IsDown(Mouse::Click::RIGHT)) { // �E�N�������Ă�
            const float rotSpeed = 0.0025f;
            rotation_.x += Mouse::GetCursorVec().y * rotSpeed;
            rotation_.y += Mouse::GetCursorVec().x * rotSpeed;
        }

        // ���s�ړ�
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && Mouse::IsDown(Mouse::Click::CENTER)) { // �E�N�������ĂȂ� && �z�C�[�������Ă�
            const float moveSpeed = 0.05f;
            eye_ += coordinate_.GetAxisX().normalize() * -mouseVelocity.x * moveSpeed;
            eye_ += coordinate_.GetAxisY().normalize() * mouseVelocity.y * moveSpeed;
        }

        // �O��ړ�
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && !Mouse::IsDown(Mouse::Click::CENTER)) { // �E�N�������ĂȂ� && �z�C�[�������ĂȂ�
            const float moveSpeed = 0.01f;
            eye_ += coordinate_.GetAxisZ().normalize() * Mouse::GetScroll() * moveSpeed;
        }
    }

    if (isFollow_) {
        matView_ = Math::Matrix::ViewLookAtLH(eye_, *targetPtr_, up_);
    }
    else {
        if (eyeDirection_.x || eyeDirection_.y || eyeDirection_.z) {
            matView_ = Math::Matrix::ViewLookToLH(coordinate_.GetPosition(), eyeDirection_.normalize(), up_);
        }
        else {
            matView_ = Math::Matrix::ViewLookToLH(coordinate_.GetPosition(), coordinate_.GetAxisZ(), up_);
        }
    }

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
    // null�`�F�b�N
    if (current_) {
        // �g�p�̗L���Ɋ֌W�Ȃ��f�o�b�O�J�������[�h��OFF
        current_->SetIsDebugMode(false);
    }

    // �V�K�J�������Z�b�g
    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic();
    Object3D::UpdateCBMatViewPerse();
}
