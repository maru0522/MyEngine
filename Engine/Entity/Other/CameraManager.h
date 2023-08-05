#pragma once
#include <memory>
#include "Vector3.h"
#include "Matrix4.h"
#include "WorldCoordinate.h"

class Camera
{
public:
    // 関数
    Camera(void) : Camera(Vector3{ 0.f,0.f,0.f }) {}
    Camera(const Vector3& pos);
    virtual ~Camera(void) = default;
    virtual void Update(void);

    //** cameraManager用
    void UpdateOrthoGraphic(void);

private:
    Vector3 Move(void);

protected:
    // 変数
    WorldCoordinate2 coordinate_;

    Matrix4 matProj_OrthoGraphic_;
    Matrix4 matProj_Perspective_;
    Matrix4 matView_;
    float nearZ_;
    float farZ_;

    Vector3* targetPtr_;
    bool isFollow_;

    bool isDebugMode_{};

public:
    // setter
    void Follow(Vector3* targetPosPtr);
    void UnFollow(void);

    inline void SetIsDebugMode(bool isDebug) { isDebugMode_ = isDebug; }

    // getter
    inline WorldCoordinate2* GetCoordinatePtr(void) { return &coordinate_; }

    inline const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }
    inline const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    inline const Matrix4& GetMatView(void) { return matView_; }

    inline float GetNearZ(void) { return nearZ_; }
    inline float GetFarZ(void) { return farZ_; }

    inline bool GetIsFollow(void) { return isFollow_; }
};

class CameraManager
{
private:
    // Singleton
    CameraManager(void) : current_(nullptr) {};
    ~CameraManager(void) {};
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

    // 変数
    Camera* current_;

public:
    // 関数
    static CameraManager* GetInstance(void);

    void Update(void);

    // setter
    void SetCurrentCamera(Camera* cameraPtr);

    // getter
    inline Camera* GetCurrentCamera(void) { return current_; }
};
