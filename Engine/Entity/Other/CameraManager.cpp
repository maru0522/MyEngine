#include "CameraManager.h"
#include "Object3D.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "Sprite.h"
#include "Input.h"

using namespace Math;
using namespace Input;

Camera::Camera(const Vector3& pos) :
    nearZ_(0.1f), farZ_(10000.f),
    targetPtr_(nullptr), isFollow_(false)
{
    transform_.position = pos;

    axes_.forward = { 0,0,1 };
    axes_.right = { 1,0,0 };
    axes_.up = { 0,1,0 };
    UpdateOrthoGraphic();
}

void Camera::Update(void)
{
    Vector3 velocity = Move();
    Vector3 currentPos = transform_.position;
    Vector3 rotation = transform_.rotation;
    // �悭�l�����炱��1Frame�O�̎p������ˁH
    Vector3 eyeDir = coordinate_.GetMatAxisZ();
    Vector3 right = coordinate_.GetMatAxisX();
    Vector3 up = coordinate_.GetMatAxisY();

    // debugCamera
    if (isDebugMode_) {
        const Vector2& mouseVelocity = Mouse::GetCursorVec();

        // ��]
        if (Mouse::IsDown(Mouse::Click::RIGHT)) { // �E�N�������Ă�
            const float rotSpeed = 0.0025f;
            rotation.x += Mouse::GetCursorVec().y * rotSpeed;
            rotation.y += Mouse::GetCursorVec().x * rotSpeed;
        }

        // ���s�ړ�
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && Mouse::IsDown(Mouse::Click::CENTER)) { // �E�N�������ĂȂ� && �z�C�[�������Ă�
            const float moveSpeed = 0.05f;
            velocity += coordinate_.GetMatAxisX().Normalize() * -mouseVelocity.x * moveSpeed;
            velocity += coordinate_.GetMatAxisY().Normalize() * mouseVelocity.y * moveSpeed;
        }

        // �O��ړ�
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && !Mouse::IsDown(Mouse::Click::CENTER)) { // �E�N�������ĂȂ� && �z�C�[�������ĂȂ�
            const float moveSpeed = 0.01f;
            velocity += coordinate_.GetMatAxisZ().Normalize() * Mouse::GetScroll() * moveSpeed;
        }
    }

    transform_.position = currentPos + velocity;
    transform_.rotation = rotation;
    axes_.forward = eyeDir;
    axes_.right = right;
    axes_.up = up;
    coordinate_.mat_world = Math::Function::AffinTrans(transform_);

    // �r���[�s��
    isFollow_ ?
        matView_ = Math::Mat4::ViewLookAtLH(transform_.position, *targetPtr_, coordinate_.GetMatAxisY()):
        matView_ = Math::Mat4::ViewLookToLH(transform_.position, axes_.forward, axes_.up = up);

    // �ˉe�s��
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}

void Camera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Mat4::ProjectionOrthoGraphicLH(WndAPI::kWidth_, WndAPI::kHeight_);
}

Vector3 Camera::Move(void)
{
    Vector3 velocity;

    //if (KEYS::IsDown(DIK_UP)) velocity.z += 1;
    //if (KEYS::IsDown(DIK_DOWN)) velocity.z -= 1;
    //if (KEYS::IsDown(DIK_LEFT)) velocity.x -= 1;
    //if (KEYS::IsDown(DIK_RIGHT)) velocity.x += 1;
    //if (KEYS::IsDown(DIK_SPACE)) velocity.y += 1;
    //if (KEYS::IsDown(DIK_LSHIFT)) velocity.y -= 1;

    return velocity;
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
    Sprite::UpdateCBMatOrthoGraphic(this);
    Object3D::UpdateCBMatViewPerse(this);
}
