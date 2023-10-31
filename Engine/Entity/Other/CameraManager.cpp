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

    // デバッグによって変更された値
    transform_.position = currentPos + velocity;
    transform_.rotation = rotation;
    // position, rotation, scale の情報のみからワールド行列を生成
    coordinate_.mat_world = Math::Function::AffinTrans(transform_);

    // 行列から各軸の向きを抽出し、それをカメラの向きとして適用
    axes_.forward = coordinate_.GetMatAxisZ();
    axes_.right = coordinate_.GetMatAxisX();
    axes_.up = coordinate_.GetMatAxisY();

    // ビュー行列
    isFollow_ ?
        matView_ = Math::Mat4::ViewLookAtLH(transform_.position, *targetPtr_, coordinate_.GetMatAxisY()):
        matView_ = Math::Mat4::ViewLookToLH(transform_.position, axes_.forward, axes_.up);

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
    // nullチェック
    if (current_) {
        // 使用の有無に関係なくデバッグカメラモードはOFF
        current_->SetIsDebugMode(false);
    }

    // 新規カメラをセット
    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic(this);
    Object3D::UpdateCBMatViewPerse(this);
}
