#pragma once
#include <memory>
#include "Vector3.h"
#include "Matrix4.h"
#include "Transform.h"
#include <unordered_map>
#include <string>

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

protected:
    // 変数
    TransformMatrix coordinate_;
    Transform transform_;
    Axis3 axes_;


    Matrix4 matProj_OrthoGraphic_;
    Matrix4 matProj_Perspective_;
    Matrix4 matView_;
    float nearZ_;
    float farZ_;

    Vector3* targetPtr_;
    bool isFollow_;
    std::string id_;

public:
    // setter
    void Follow(Vector3* targetPosPtr);
    void UnFollow(void);

    // getter
    inline TransformMatrix* GetCoordinatePtr(void) { return &coordinate_; }
    Transform* GetTransformPtr(void) { return &transform_; }
    Axis3* GetAxis3Ptr(void) { return &axes_; }

    inline const Matrix4& GetMatProjOrthoGraphic(void) { return matProj_OrthoGraphic_; }
    inline const Matrix4& GetMatProjPerspective(void) { return matProj_Perspective_; }
    inline const Matrix4& GetMatView(void) { return matView_; }

    inline float GetNearZ(void) { return nearZ_; }
    inline float GetFarZ(void) { return farZ_; }

    inline bool GetIsFollow(void) { return isFollow_; }
};

class DebugCamera final : public Camera
{
public:
    //>> 関数
    DebugCamera(void) : Camera() {}
    DebugCamera(const Vector3& arg_pos) : Camera(arg_pos) {}
    virtual ~DebugCamera(void) = default;
    virtual void Update(void);
};

class CameraManager final
{
private:
    //>> Singleton
    CameraManager(void) : current_(nullptr), is_finalized_(false) {};
    ~CameraManager(void) {};
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

    //>> 定義
    using KEY = std::string;

public:
    //>> 関数
    static CameraManager* GetInstance(void);
    
    void Initialize(void);
    void Update(void);
    void Finalize(void);

    // 一般的なカメラの登録
    void Register(const std::string& arg_key, Camera* arg_cameraPtr);
    void UnRegister(const std::string& arg_key);
    // デバッグカメラの生成
    void CreateDebugCamera(Camera* arg_cameraPtr);
    void DestroyDebugCamera(const std::string& arg_key);

private:
    void RecursiveSearchKey(std::string& arg_key, int32_t& arg_duplicateNum);

    //>> 変数
    bool is_finalized_;
    bool is_switchCameraByCreateDebugCam_;

    Camera* current_;
    std::unordered_map<KEY, Camera*> umap_cameras_;
    std::unordered_map<KEY, std::unique_ptr<DebugCamera>> umap_debugCameras_;

public:
    //>> setter
    void SetCurrentCamera(const std::string& arg_key);

    //>> getter
    inline Camera* GetCurrentCamera(void) { return current_; }
};
