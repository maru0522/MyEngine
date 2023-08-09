#pragma once
#include <memory>
#include "Vector3.h"
#include "Matrix4.h"
#include "Transform.h"
#include "WorldCoordinate.h"

class Camera
{
public:
    // ä÷êî
    Camera(void) : Camera(Vector3{ 0.f,0.f,0.f }) {}
    Camera(const Vector3& pos);
    virtual ~Camera(void) = default;
    virtual void Update(void);

    //** cameraManageróp
    void UpdateOrthoGraphic(void);

private:
    Vector3 Move(void);

protected:
    // ïœêî
    Coordinate coordinate_;
    Transform transform_;
    Axis3 axes_;


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
    inline Coordinate* GetCoordinatePtr(void) { return &coordinate_; }
    Transform* GetTransformPtr(void) { return &transform_; }
    Axis3* GetAxis3Ptr(void) { return &axes_; }

    inline const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }
    inline const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    inline const Matrix4& GetMatView(void) { return matView_; }

    inline float GetNearZ(void) { return nearZ_; }
    inline float GetFarZ(void) { return farZ_; }

    inline bool GetIsFollow(void) { return isFollow_; }
};

class CameraManager final
{
private:
    // Singleton
    CameraManager(void) : current_(nullptr) {};
    ~CameraManager(void) {};
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

    // ïœêî
    Camera* current_;

public:
    // ä÷êî
    static CameraManager* GetInstance(void);

    void Update(void);

    // setter
    void SetCurrentCamera(Camera* cameraPtr);

    // getter
    inline Camera* GetCurrentCamera(void) { return current_; }
};
