#pragma once
#include "GraphicsPipeline.h"
#include "WorldCoordinate.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "ConstBuffer.h"
#include "FileSystem.h"
#include "Matrix4.h"

class Object3D
{
private:
    // 定義
    using fsPath = std::experimental::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData3d_t // 定数バッファ用"データ"構造体
    {
        Matrix4 matWorld_; // 3D行列変換
    };

    struct CBMatViewPerse_t // 定数バッファ用"射影行列"構造体
    {
        Matrix4 matView_;
        Matrix4 matPerspective_;
    };

public:
    //enum class Type
    //{
    //    MUTABLE, // 可変 - 外観変更があるもの（例：自機とか敵とかのモデル）
    //    IMMUTABLE, // 不変 - 外観変更がないもの（例：建物とか天球とか装飾系のモデル）
    //};

    // 関数
    static void StaticInitialize(ModelManager* modelMPtr, TextureManager* texMPtr, CameraManager* camMPtr);
    static void UpdateCBMatViewPerse(void);
    static void PreDraw(BlendMode blendmode = BlendMode::ALPHA);

    Object3D(const fsPath& path/*,Type type*/);
    void Update(void);
    void Draw(void);
    void Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture);
    void Draw(const fsPath& path);

    WorldCoordinate coordinate_;
private:
    // 変数
    Object3D* parent_;

    Model_t model_;

    ConstBuffer<CBData3d_t> cb_;

    static ModelManager* modelMPtr_;
    static TextureManager* texMPtr_;
    static CameraManager* camMPtr_;
    static ConstBuffer<CBMatViewPerse_t> cbMatViewPerse_;
};

