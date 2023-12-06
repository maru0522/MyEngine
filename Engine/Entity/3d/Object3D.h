#pragma once
#include "TextureManager.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "ConstBuffer.h"
#include "PSOManager.h"
#include <filesystem>
#include "Matrix4.h"
#include "LightManager.h"
#include "Transform.h"

class Object3D
{
private:
    // 定義
    using fsPath = std::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData3d_t // 定数バッファ用"データ"構造体
    {
        Matrix4 matWorld; // 3D行列変換
        bool isShadowFlash;
    };

    struct CBMatViewPerse_t // 定数バッファ用"射影行列"構造体
    {
        Matrix4 matView;
        Matrix4 matPerspective;
        Vector3 cameraPos;
    };

public:
    // 関数
    static void StaticInitialize(ModelManager* modelMPtr, TextureManager* texMPtr);
    // 透視投影行列を任意のタイミングで更新するための関数
    static void UpdateCBMatViewPerse(CameraManager* camMPtr);

    static void PreDraw(BlendMode blendmode = BlendMode::ALPHA);
    static void SetDrawBlendMode(BlendMode blendmode);

    Object3D(const fsPath& path/*,Type type*/);
    virtual ~Object3D(void) = default;

    virtual void Update(void);
    virtual void Draw(void);
    void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE& texture);
    void Draw(const fsPath& path);

protected:
    // 変数
    Object3D* parent_;

    TransformMatrix matTrans_;
    ConstBuffer<CBData3d_t> cb_;
    bool isInvisible_{};
    bool isShadowFlash_{};
    Model_t model_;

    static LightManager* sLightGroupPtr_;
    static ModelManager* sModelMPtr_;
    static TextureManager* sTexMPtr_;
    static ConstBuffer<CBMatViewPerse_t> sCbMatViewPerse_;

public:
    // setter・getter
    static void SetLightGroup(LightManager* lightGroupPtr) { sLightGroupPtr_ = lightGroupPtr; }

    void SetIsInvisible(bool arg_isInvisible) { isInvisible_ = arg_isInvisible; }
    void SetTransformMatrix(const TransformMatrix& arg_coordinate) { matTrans_ = arg_coordinate; }
    void SetIsShadowFlash(bool arg_isShadowFlash) { isShadowFlash_ = arg_isShadowFlash; }
    void SetMatWorld(const Matrix4& arg_matWorld) { matTrans_.mat_world = arg_matWorld; }

    TransformMatrix* GetCoordinatePtr(void) { return &matTrans_; }
};

