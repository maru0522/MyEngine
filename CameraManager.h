#pragma once
#include <memory>
#include "Vector3.h"
#include "Matrix4.h"
#include "Sprite.h"
#include "Object3D.h"

class Camera
{
public:
    // 関数
    Camera(void) : Camera(Vector3{ 0.f,0.f,0.f }) {}
    Camera(const Vector3& eye);
    void Update(void);

    void UpdateOrthoGraphic(void);
private:

    // 変数
    Vector3 eye_;
    Vector3 up_;
    Vector3 rotation_;

    Vector3* targetPtr_;
    bool isFollow_;

    float nearZ_;
    float farZ_;

    Matrix4 matProj_OrthoGraphic_;
    Matrix4 matProj_Perspective_;
    Matrix4 matView_;


public:
    // getter・setter
    inline void SetPosition(const Vector3& eye) { eye_ = eye; }
    inline void SetUp(const Vector3& up) { up_ = up; }
    inline void SetNearZ(float nearZ) { nearZ_ = nearZ; }
    inline void SetFarZ(float farZ) { farZ_ = farZ; }

    inline const Vector3& GetPosition(void) { return eye_; }
    inline const Vector3& GetUp(void) { return up_; }
    inline float GetNearZ(void) { return nearZ_; }
    inline float GetFarZ(void) { return farZ_; }

    inline const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }
    inline const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    inline const Matrix4& GetMatView(void) { return matView_; }

    inline void Follow(Vector3* p_target) { isFollow_ = true, targetPtr_ = p_target; }
    inline void UnFollow(void) { isFollow_ = false; }
    inline bool GetIsFollow(void) { return isFollow_; }
    inline Vector3* GetFollowPosPtr(void) { return targetPtr_; }
};

class CameraManager
{
public:
    // 関数
    static CameraManager* GetInstance(void);

    
    void Update(void);

private:
    // 変数
    Camera* current_;

public:
    // getter・setter
    void SetCurrentCamera(Camera* cameraPtr);

    inline Camera* GetCurrentCamera(void) { return current_; }

private:
    // Singleton
    CameraManager(void) : current_(nullptr) {};
    ~CameraManager(void) {};
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;
};

