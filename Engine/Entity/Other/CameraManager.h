#pragma once
#include <memory>
#include "ICamera.h"
#include "DebugCamera.h"

class CameraManager final
{
private:
    //>> Singleton
    CameraManager(void) : current_(nullptr), is_finalized_(false) {};
    ~CameraManager(void) {};
    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

public:
    //>> 関数
    static CameraManager* GetInstance(void);
    
    void Initialize(void);
    void Update(void);
    void Finalize(void);

    // 一般的なカメラの登録
    void Register(ICamera* arg_cameraPtr);
    // 一般的なカメラの抹消
    void UnRegister(ICamera* arg_cameraPtr);
    void UnRegister(const std::string& arg_id);
    // デバッグカメラの生成
    void CreateDebugCamera(ICamera* arg_cameraPtr);
    // デバッグカメラの削除
    void DestroyDebugCamera(const std::string& arg_id);

private:
    void DebugGui(void);

    // ベクタ配列に登録されたカメラの名前に被りがあるか検索（カメラ用）
    bool IsContainSameId_Camera(const std::string& arg_id);
    // ベクタ配列に登録されたカメラの名前に被りがあるか検索（デバッグカメラ用）
    bool IsContainSameId_DebugCamera(const std::string& arg_id);
    // 再帰的に名前検索と末尾の数字加算を繰り返し、名前被り無く登録できるようにする（カメラ用）
    void RecursiveSearchId_Camera(std::string& arg_id, int32_t& arg_duplicateNum);
    // 再帰的に名前検索と末尾の数字加算を繰り返し、名前被り無く登録できるようにする（デバッグカメラ用）
    void RecursiveSearchId_DebugCamera(std::string& arg_id, int32_t& arg_duplicateNum);

    //>> 変数
    bool is_finalized_;
    bool is_switchCameraByCreateDebugCam_;

    ICamera* current_;
    std::vector<ICamera*> vector_cameras_;
    std::vector<std::unique_ptr<DebugCamera>> vector_debugCameras_;

public:
    //>> setter
    void SetCurrentCamera(ICamera* arg_cameraPtr);
    void SetCurrentCamera(const std::string& arg_id);

    //>> getter
    inline ICamera* GetCurrentCamera(void) { return current_; }
};
