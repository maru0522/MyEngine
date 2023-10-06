#pragma once
#include <array>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

#include <d3d12.h>

#include "TextureManager.h"
#include "CameraManager.h"
#include "VertexBuffer.h"
#include "ConstBuffer.h"
#include "PSOManager.h"
#include "FileSystem.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "WndAPI.h"

class Sprite
{
private:
    // 定義
    using fsPath = std::experimental::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
    struct CBMatOrthoGraphic_t // 定数バッファ用"射影行列"構造体
    {
        Matrix4 matOrthoGraphic;
    };

    struct CBData2d_t // 定数バッファ用"データ"構造体
    {
        Matrix4 matWorld; // 3D変換行列
        Vector4 color; // 色（RGBA）
    };

    struct VertexPosUv_t // 頂点バッファ用"データ"構造体
    {
        Vector3 pos; // xyz座標
        Vector2 uv; // uv座標
    };

    // 関数
    static void StaticInitialize(TextureManager* texMPtr);
    static void PreDraw(BlendMode blendmode = BlendMode::ALPHA);
    static void SetDrawBlendMode(BlendMode blendmode);

    // 平行投影行列を任意のタイミングで更新するための関数
    static void UpdateCBMatOrthoGraphic(CameraManager* camMPtr);

    Sprite(const fsPath& path, const std::string& nickname = "noAssign"); // nickNameが優先される。
    void Update(void);
    void Draw(void);

protected:
    void TransferVertex(void);
    void UpdateMatrix(void);

protected:
    // 変数
    static TextureManager* sTexMPtr_;
    static ConstBuffer<CBMatOrthoGraphic_t> sCbMatOrthoGraphic_;

    Sprite* parent_;

    Vector2 position_;
    Vector2 scale_;
    Vector2 size_;
    float rotation_;
    Matrix4 matWorld_;

    Vector2 anchorPoint_;
    Vector2 cutStartPoint_;
    Vector2 cutLength_;

    bool isFlipX_;
    bool isFlipY_;
    bool isInvisible_;

    ConstBuffer<CBData2d_t> cb_;
    VertexBuffer<VertexPosUv_t> vertexBuffer_;

    const TextureManager::Image* imagePtr_;

public:
    // setter・getter
    inline const Vector2& GetPosition(void) { return position_; }
    inline const Vector2& GetScale(void) { return scale_; }
    inline const Vector2& GetSize(void) { return size_; }
    inline float GetRotation(void) { return rotation_; }
    inline const Vector2& GetAnchorPoint(void) { return anchorPoint_; }
    inline const Vector2& GetCutStartPoint(void) { return cutStartPoint_; }
    inline const Vector2& GetCutLength(void) { return cutLength_; }
    inline bool  GetFlipX(void) { return isFlipX_; }
    inline bool  GetFlipY(void) { return isFlipY_; }
    inline bool  GetInvisible(void) { return isInvisible_; }

    inline void SetParent(Sprite* spritePtr) { parent_ = spritePtr; }
    inline void SetPosition(const Vector2& posotion) { position_ = posotion; }
    inline void SetScale(const Vector2& scale) { scale_ = scale; }
    inline void SetSize(const Vector2& size) { size_ = size; }
    inline void SetRotation(float rotation) { rotation_ = rotation; }
    inline void SetAnchorPoint(const Vector2& anchor) { anchorPoint_ = anchor; }
    inline void SetCutStartPoint(const Vector2& cutStart) { cutStartPoint_ = cutStart; }
    inline void SetCutLength(const Vector2& cutLength) { cutLength_ = cutLength; }
    inline void SetFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
    inline void SetFlipY(bool isFlipY) { isFlipY_ = isFlipY; }
    inline void SetInvisible(bool isInvisible) { isInvisible_ = isInvisible; }

    void SetColor(const Vector4& rgba);
    void SetColor255(const Vector4& rgba);
};

