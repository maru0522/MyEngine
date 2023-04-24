#pragma once
#include <memory>
#include "Vector3.h"
#include "Matrix4.h"

class Camera
{
public:
    // ä÷êî
    Camera(void);
    Camera(const Vector3& eye);
    void Update(void);

    void UpdateOrthoGraphic(void);
private:

    // ïœêî
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
    // getterÅEsetter
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

    inline void Follow(Vector3* p_target) { isFollow_ = true, targetPtr_ = p_target; }
    inline void UnFollow(void) { isFollow_ = false; }
    inline bool GetIsFollow(void) { return isFollow_; }
    inline Vector3* GetFollowPosPtr(void) { return targetPtr_; }
};

class CameraManager
{
public:
    // ä÷êî
    CameraManager(void) : current_(nullptr) {};
    void Update(void);

private:
    // ïœêî
    Camera* current_;

public:
    // getterÅEsetter
    void SetCurrentCamera(Camera* cameraPtr);

    inline Camera* GetCurrentCamera(void) { return current_; }
};

