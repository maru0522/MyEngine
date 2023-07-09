#include "WorldCoordinate.h"
#include "CameraManager.h"
#include "MathUtil.h"
#include "WndAPI.h"
#include "Input.h"

using namespace Math;
using namespace Input;

Camera::Camera(const Vector3& pos) :
    coordinate_(pos, Vector3{ 1.f,1.f,1.f }, Vector3{ 0.f,0.f,0.f }),
    nearZ_(0.1f), farZ_(1000.f),
    targetPtr_(nullptr), isFollow_(false)
{
    coordinate_.SetAxisUp({ 0,1,0,0 });
    coordinate_.SetAxisForward({ 0,0,1,0 });
    UpdateOrthoGraphic();
}

void Camera::Update(void)
{
    Vector3 currentPos = coordinate_.GetPosition();
    Vector3 velocity = Move();
    Vector3 rotation{};
    Quaternion eyeDir = coordinate_.GetForwardVec();
    Quaternion up = coordinate_.GetUpVec();

    // debugCamera
    if (isDebugMode_) {
        const Vector2& mouseVelocity = Mouse::GetCursorVec();

        // 回転
        if (Mouse::IsDown(Mouse::Click::RIGHT)) { // 右クリ押してる
            const float rotSpeed = 0.0025f;
            rotation.x += Mouse::GetCursorVec().y * rotSpeed;
            rotation.y += Mouse::GetCursorVec().x * rotSpeed;
        }

        // 平行移動
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && Mouse::IsDown(Mouse::Click::CENTER)) { // 右クリ押してない && ホイール押してる
            const float moveSpeed = 0.05f;
            velocity += coordinate_.GetMatAxisX().Normalize() * -mouseVelocity.x * moveSpeed;
            velocity += coordinate_.GetMatAxisY().Normalize() * mouseVelocity.y * moveSpeed;
        }

        // 前後移動
        if (!Mouse::IsDown(Mouse::Click::RIGHT) && !Mouse::IsDown(Mouse::Click::CENTER)) { // 右クリ押してない && ホイール押してない
            const float moveSpeed = 0.01f;
            velocity += coordinate_.GetMatAxisZ().Normalize() * Mouse::GetScroll() * moveSpeed;
        }
    }

    // WorldCoordinateを常にリセットし続けるのは使う情報を一時保存しておかなければならない誓約上不便かもしれない
    coordinate_.Reset();
    coordinate_.SetPosition(currentPos + velocity);
    coordinate_.SetRotation(rotation);
    coordinate_.SetAxisForward(eyeDir);
    coordinate_.SetAxisUp(up);
    coordinate_.Update();

    // ビュー行列
    isFollow_ ?
        matView_ = Math::Mat4::ViewLookAtLH(coordinate_.GetPosition(), *targetPtr_, coordinate_.GetMatAxisY()):
        matView_ = Math::Mat4::ViewLookToLH(coordinate_.GetPosition(), coordinate_.GetForwardVec().ExtractVector3().Normalize(), coordinate_.GetUpVec().ExtractVector3().Normalize());

    // 射影行列
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}

void Camera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Mat4::ProjectionOrthoGraphicLH(WndAPI::kWidth_, WndAPI::kHeight_);
}

Vector3 Camera::Move(void)
{
    Vector3 velocity;

    if (KEYS::IsDown(DIK_UP)) velocity.z += 1;
    if (KEYS::IsDown(DIK_DOWN)) velocity.z -= 1;
    if (KEYS::IsDown(DIK_LEFT)) velocity.x -= 1;
    if (KEYS::IsDown(DIK_RIGHT)) velocity.x += 1;
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
    // nullチェック
    if (current_) {
        // 使用の有無に関係なくデバッグカメラモードはOFF
        current_->SetIsDebugMode(false);
    }

    // 新規カメラをセット
    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic();
    Object3D::UpdateCBMatViewPerse();
}
