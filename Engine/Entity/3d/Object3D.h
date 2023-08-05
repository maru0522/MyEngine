#pragma once
#include "WorldCoordinate.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "ConstBuffer.h"
#include "PSOManager.h"
#include "FileSystem.h"
#include "Matrix4.h"
#include "LightGroup.h"

class Object3D
{
private:
    // 定義
    using fsPath = std::experimental::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData3d_t // 定数バッファ用"データ"構造体
    {
        Matrix4 matWorld; // 3D行列変換
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
    void Update(void);
    void Draw(void);
    void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE& texture);
    void Draw(const fsPath& path);

private:
    // 変数
    Object3D* parent_;

    Model_t model_;
    bool isInvisible_{};
    WorldCoordinate2 coordinate_;
    ConstBuffer<CBData3d_t> cb_;

    static LightGroup* sLightGroupPtr_;
    static ModelManager* sModelMPtr_;
    static TextureManager* sTexMPtr_;
    static ConstBuffer<CBMatViewPerse_t> sCbMatViewPerse_;

public:
    // setter・getter
    static inline void SetLightGroup(LightGroup* lightGroupPtr) { sLightGroupPtr_ = lightGroupPtr; }

    inline void SetIsInvisible(bool isInvisible) { isInvisible_ = isInvisible; }
    inline void SetCoordinate(const WorldCoordinate2& coordinate) { coordinate_ = coordinate; }

    inline const WorldCoordinate2& GetCoordinate(void) { return coordinate_; }
    inline WorldCoordinate2* GetCoordinatePtr(void) { return &coordinate_; }
};

