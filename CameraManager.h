#pragma once
#include <memory>
#include "Vector3.h"
#include "Matrix4.h"

class Camera
{
public:
    // ŠÖ”
    Camera(void);
    Camera(const Vector3& eye, const Vector3& target, const Vector3& up = { 0.f,1.f,0.f });
    void Update(void);

private:
    // •Ï”
    Vector3 eye_;
    Vector3 target_;
    Vector3 up_;

    float nearZ_;
    float farZ_;

    Matrix4 matProj_OrthoGraphic_;
    Matrix4 matProj_Perspectivee_;

public:
    // getterEsetter
    inline void SetEye(const Vector3& eye) { eye_ = eye; }
    inline void SetTarget(const Vector3& target) { target_ = target; }
    inline void SetUp(const Vector3& up) { up_ = up; }
    inline void SetNearZ(float nearZ) { nearZ_ = nearZ; }
    inline void SetFarZ(float farZ) { farZ_ = farZ; }

    inline const Vector3& GetEye(void) { return eye_; }
    inline const Vector3& GetTarget(void) { return target_; }
    inline const Vector3& GetUp(void) { return up_; }
    inline float GetNearZ(void) { return nearZ_; }
    inline float GetFarZ(void) { return farZ_; }
};

class CameraManager
{
public:
    // ŠÖ”
    CameraManager(void) {};
    void Update(void);

private:
    // •Ï”
    static Camera* current_;

public:
    // getterEsetter
    inline void SetCamera(Camera* cameraPtr) { current_ = cameraPtr; }
};

