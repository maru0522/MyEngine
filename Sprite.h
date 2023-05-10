#pragma once
#include <array>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include "GraphicsPipeline.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "VertexBuffer.h"
#include "ConstBuffer.h"
#include "FileSystem.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "WndAPI.h"

class Sprite
{
private:
    // ��`
    using fsPath = std::experimental::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData2d_t // Sprite�p�萔�o�b�t�@�̃f�[�^�\����
    {
        Matrix4 matWorld_; // 3D�ϊ��s��
        Vector4 color_; // �F�iRGBA�j
    };

    struct CBMatOrthoGraphic_t // Sprite�p�萔�o�b�t�@
    {
        Matrix4 matOrthoGraphic_;
    };

    struct VertexPosUv_t
    {
        Vector3 pos; // xyz���W
        Vector2 uv; // uv���W
    };

public:
    // �֐�
    static void StaticInitialize(TextureManager* texMPtr, CameraManager* camMPtr);
    static void PreDraw(BlendMode blendmode = BlendMode::ALPHA);
    static void SetDrawBlendMode(BlendMode blendmode);

    static inline void UpdateCBMatOrthoGraphic(void) { cbMatOrthoGraphic_.GetConstBuffMap()->matOrthoGraphic_ = camMPtr_->GetCurrentCamera()->GetMatProjOrthoGraphic(); }

    Sprite(const fsPath& path, const std::string& nickname = "noAssign"); // nickName���D�悳���B
    void Update(void);
    void Draw(void);

private:
    void TransferVertex(void);
    void UpdateMatrix(void);

    // �ϐ�
    static TextureManager* texMPtr_;
    static CameraManager* camMPtr_;
    static ConstBuffer<CBMatOrthoGraphic_t> cbMatOrthoGraphic_;

    Sprite* parent_;

    Vector2 position_;
    Vector2 scale_;
    Vector2 size_;
    float rotation_;

    Vector2 anchorPoint_;
    Vector2 cutStartPoint_;
    Vector2 cutLength_;

    bool isFlipX_;
    bool isFlipY_;
    bool isInvisible_;

    Matrix4 matWorld_;

    ConstBuffer<CBData2d_t> cb_;
    VertexBuffer<VertexPosUv_t> vertexBuffer_;

    const TextureManager::Image* imagePtr_;

public:
    // setter�Egetter
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

    void SetColor(Vector4 rgba);
    void SetColor255(Vector4 rgba);
};

