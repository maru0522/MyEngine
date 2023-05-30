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
    // ��`
    using fsPath = std::experimental::filesystem::path;
    using BlendMode = HelperGraphicPipeline::BlendMode;
    template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct CBData3d_t // �萔�o�b�t�@�p"�f�[�^"�\����
    {
        Matrix4 matWorld_; // 3D�s��ϊ�
    };

    struct CBMatViewPerse_t // �萔�o�b�t�@�p"�ˉe�s��"�\����
    {
        Matrix4 matView_;
        Matrix4 matPerspective_;
    };

public:
    //enum class Type
    //{
    //    MUTABLE, // �� - �O�ϕύX��������́i��F���@�Ƃ��G�Ƃ��̃��f���j
    //    IMMUTABLE, // �s�� - �O�ϕύX���Ȃ����́i��F�����Ƃ��V���Ƃ������n�̃��f���j
    //};

    // �֐�
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
    // �ϐ�
    Object3D* parent_;

    Model_t model_;

    ConstBuffer<CBData3d_t> cb_;

    static ModelManager* modelMPtr_;
    static TextureManager* texMPtr_;
    static CameraManager* camMPtr_;
    static ConstBuffer<CBMatViewPerse_t> cbMatViewPerse_;
};

