#pragma once
#include "GraphicsPipeline.h"
#include "WorldCoordinate.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "ModelManager.h"
#include "ConstBuffer.h"
#include "FileSystem.h"
#include "Matrix4.h"
#include "LightGroup.h"

class Object3D
{
private:
    // ��`
    using fsPath = std::experimental::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData3d_t // �萔�o�b�t�@�p"�f�[�^"�\����
    {
        Matrix4 matWorld; // 3D�s��ϊ�
    };

    struct CBMatViewPerse_t // �萔�o�b�t�@�p"�ˉe�s��"�\����
    {
        Matrix4 matView;
        Matrix4 matPerspective;
        Vector3 cameraPos;
    };

public:
    // �֐�
    static void StaticInitialize(ModelManager* modelMPtr, TextureManager* texMPtr);
    // �������e�s���C�ӂ̃^�C�~���O�ōX�V���邽�߂̊֐�
    static void UpdateCBMatViewPerse(void);

    static void PreDraw(BlendMode blendmode = BlendMode::ALPHA);
    static void SetDrawBlendMode(BlendMode blendmode);

    Object3D(const fsPath& path/*,Type type*/);
    void Update(void);
    void Draw(void);
    void Draw(const D3D12_GPU_DESCRIPTOR_HANDLE& texture);
    void Draw(const fsPath& path);

    WorldCoordinate coordinate_;
private:
    // �ϐ�
    Object3D* parent_;

    Model_t model_;

    ConstBuffer<CBData3d_t> cb_;

    static LightGroup* sLightGroupPtr_;
    static ModelManager* sModelMPtr_;
    static TextureManager* sTexMPtr_;
    static ConstBuffer<CBMatViewPerse_t> sCbMatViewPerse_;

public:
    // setter�Egetter
    static inline void SetLightGroup(LightGroup* lightGroupPtr) { sLightGroupPtr_ = lightGroupPtr; }
};

