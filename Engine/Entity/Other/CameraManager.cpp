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
    // position, rotation, scale の情報のみからワールド行列を生成
    coordinate_.mat_world = Math::Function::AffinTrans(transform_);

    // 行列から各軸の向きを抽出し、それをカメラの向きとして適用
    axes_.forward = coordinate_.GetMatAxisZ();
    axes_.right = coordinate_.GetMatAxisX();
    axes_.up = coordinate_.GetMatAxisY();

    // ビュー行列
    isFollow_ ?
        matView_ = Math::Mat4::ViewLookAtLH(transform_.position, *targetPtr_, coordinate_.GetMatAxisY()) :
        matView_ = Math::Mat4::ViewLookToLH(transform_.position, axes_.forward, axes_.up);

    // 射影行列
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}

void Camera::UpdateOrthoGraphic(void)
{
    matProj_OrthoGraphic_ = Mat4::ProjectionOrthoGraphicLH(WndAPI::kWidth_, WndAPI::kHeight_);
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

void CameraManager::Initialize(void)
{
    is_finalized_ = false;
}

void CameraManager::Update(void)
{
    // 終了処理後ならスキップ
    if (is_finalized_) { return; }
    // nullチェック
    if (!current_) { return; }

    // 更新処理
    current_->Update();
}

void CameraManager::Finalize(void)
{
    is_finalized_ = true;

    // 現在のカメラを外す
    current_ = nullptr;

    // umap配列を初期化
    umap_cameras_.clear();
    umap_debugCameras_.clear();
}

void CameraManager::Register(const std::string& arg_key, Camera* arg_cameraPtr)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // KEYにcameraを足す
    const std::string key("Camera_" + arg_key);
    // umap配列に同名カメラptrが既に登録されているか
    bool isContained = umap_cameras_.contains(arg_key);
    // 登録されているならスキップ
    if (isContained) { return; }


    // 登録
    umap_cameras_.insert({ arg_key, arg_cameraPtr });
}

void CameraManager::UnRegister(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に存在するか
    bool isContained = umap_cameras_.contains(arg_key);
    // 存在しないならスキップ
    if (isContained == false) { return; }


    // 登録
    umap_cameras_.erase(arg_key);
}

void CameraManager::CreateDebugCamera(Camera* arg_cameraPtr)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    int32_t num{};
    // KEYにdebugcameraを足す
    std::string key("Debug" + arg_cameraPtr + std::to_string(num)); // <== DebugCamera_"key"0 からはじまる
    // umap配列に同名のデバッグカメラがあるか検索し、ある場合は末尾の番号を1つずらす。
    RecursiveSearchKey(key, num); // DebugCamera_"key"0 ~ ...


    // 登録
    umap_debugCameras_.insert({ arg_key, arg_cameraPtr });

    // デバッグカメラ生成時、使用するカメラも、そちらに切り替えるか
    if(is_switchCameraByCreateDebugCam_) {SetCurrentCamera(arg_key) }
}

void CameraManager::DestroyDebugCamera(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に存在するか
    bool isContained = umap_cameras_.contains(arg_key);
    // 存在しないならスキップ
    if (isContained == false) { return; }


    // 登録
    umap_cameras_.erase(arg_key);
}

void CameraManager::RecursiveSearchKey(std::string& arg_key, int32_t& arg_duplicateNum)
{
    bool isContained = umap_debugCameras_.contains(arg_key);
    if (isContained)
    {
        arg_key.pop_back();

        arg_duplicateNum++;
        arg_key += std::to_string(arg_duplicateNum);

        RecursiveSearchKey(arg_key,arg_duplicateNum);
    }

    return;
}

void CameraManager::SetCurrentCamera(const std::string& arg_key)
{
    // 終了処理呼んだ後ならスキップ
    if (is_finalized_) { return; }


    // umap配列に存在するか（一般カメラ）
    bool isContained_camera = umap_cameras_.contains(arg_key);
    // umap配列に存在するか（デバッグカメラ）
    bool isContained_debugCamera = umap_debugCameras_.contains(arg_key);
    // カメラクラスのポインタ
    Camera* cameraPtr = nullptr;


    // KEY がどちらにも存在しない場合、スキップ
    if (isContained_camera == false && isContained_debugCamera == false) { return; }
    // どちらか1つのみ、trueになる。（2つのumap配列で KEY の値が被ることは無い）
    if (isContained_camera) { cameraPtr = umap_cameras_.at(arg_key); }
    if (isContained_debugCamera) { cameraPtr = umap_debugCameras_.at(arg_key).get(); }


    // 新規カメラをセット
    current_ = cameraPtr;
    current_->Update();
    current_->UpdateOrthoGraphic();
    Sprite::UpdateCBMatOrthoGraphic(this);
    Object3D::UpdateCBMatViewPerse(this);
}

void DebugCamera::Update(void)
{
    Vector3 velocity{};
    Vector3 currentPos = transform_.position;
    Vector3 rotation = transform_.rotation;

    // debugCamera
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
        matView_ = Math::Mat4::ViewLookAtLH(transform_.position, *targetPtr_, coordinate_.GetMatAxisY()) :
        matView_ = Math::Mat4::ViewLookToLH(transform_.position, axes_.forward, axes_.up);

    // 射影行列
    matProj_Perspective_ = Mat4::ProjectionPerspectiveFovLH(Function::ToRadian(45.f), WndAPI::kWidth_, WndAPI::kHeight_, nearZ_, farZ_);
}
