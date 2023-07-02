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

    //** cameraManager用
    void UpdateOrthoGraphic(void);

    // 変数
    Vector3 eye_;
    Vector3 up_;
    Vector3 rotation_;

private:
    Vector3* targetPtr_;
    bool isFollow_;

    float nearZ_;
    float farZ_;

    WorldCoordinate coordinate_;
    Matrix4 matProj_OrthoGraphic_;
    Matrix4 matProj_Perspective_;
    Matrix4 matView_;

    bool isDebugMode_{};

public:
    // getter・setter
    inline const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }
    inline const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    inline Vector3 GetForwardVec(void) { return coordinate_.GetAxisZ(); }
    inline Vector3 GetRightVec(void) { return coordinate_.GetAxisX(); }
    inline const Matrix4& GetMatView(void) { return matView_; }

    void Follow(Vector3* p_target);
    void UnFollow(void);
    inline bool GetIsFollow(void) { return isFollow_; }

    inline void SetIsDebugMode(bool isDebug) { isDebugMode_ = isDebug; }
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

