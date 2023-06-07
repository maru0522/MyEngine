#include <vector>
#include "Sprite.h"
#include "Matrix4.h"
#include "InitDirectX.h"
#include "CameraManager.h"
#include "TextureManager.h"
#include "GraphicsPipeline.h"

TextureManager* Sprite::sTexMPtr_{ nullptr };
ConstBuffer<Sprite::CBMatOrthoGraphic_t> Sprite::sCbMatOrthoGraphic_{};

void Sprite::StaticInitialize(TextureManager* texMPtr)
{
    sTexMPtr_ = texMPtr;
    sCbMatOrthoGraphic_.Create();

    // 初期化時はカメラが存在しないためCameraManagerのSetCurrentCamera()時に実行するようにする
    //UpdateCBMatOrthoGraphic();
}

void Sprite::PreDraw(BlendMode blendmode)
{
    InitDirectX* iDX = InitDirectX::GetInstance();

    // プリミティブ形状の設定コマンド
    iDX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

    // SRVヒープの設定コマンド
    ID3D12DescriptorHeap* ppHeaps[] = { iDX->GetDescHeap_t()->GetDescHeap() };
    iDX->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    // パイプラインステートとルートシグネチャの設定コマンド
    iDX->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline2d(blendmode).pipelineState_.Get());
    iDX->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline2d(blendmode).rootSignature_.Get());
}

void Sprite::SetDrawBlendMode(BlendMode blendmode)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // パイプラインステートとルートシグネチャの設定コマンド
    iDXPtr->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline2d(blendmode).pipelineState_.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline2d(blendmode).rootSignature_.Get());
}

void Sprite::UpdateCBMatOrthoGraphic(void)
{
    // cameraManagerのptr取得
    CameraManager* cameraMPtr = CameraManager::GetInstance();
    // 平行投影行列にカメラの保持する平行投影行列を代入
    sCbMatOrthoGraphic_.GetConstBuffMap()->matOrthoGraphic_ = cameraMPtr->GetCurrentCamera()->GetMatProjOrthoGraphic();
}

Sprite::Sprite(const fsPath& path, const std::string& nickname) :
    parent_(nullptr),
    position_(0.f, 0.f), scale_(1.f, 1.f), size_(0.f, 0.f), rotation_(0.f),
    imagePtr_(nullptr)
{
    nickname == "noAssign" ?
        imagePtr_ = sTexMPtr_->GetImagePtr(path) :
        imagePtr_ = sTexMPtr_->GetImagePtrByNickname(nickname);

    size_.x = (float)imagePtr_->buff_->GetDesc().Width;
    size_.y = (float)imagePtr_->buff_->GetDesc().Height;

    cutLength_.x = (float)imagePtr_->buff_->GetDesc().Width;
    cutLength_.y = (float)imagePtr_->buff_->GetDesc().Height;

    // 定数バッファ生成
    cb_.Create();

    std::vector<VertexPosUv_t> vertices;
    vertices.emplace_back(VertexPosUv_t{ {   0.0f, 100.0f, 0.0f }, {0.0f, 1.0f} }); // 左下
    vertices.emplace_back(VertexPosUv_t{ {   0.0f,   0.0f, 0.0f }, {0.0f, 0.0f} }); // 左上
    vertices.emplace_back(VertexPosUv_t{ { 100.0f, 100.0f, 0.0f }, {1.0f, 1.0f} }); // 右下
    vertices.emplace_back(VertexPosUv_t{ { 100.0f,   0.0f, 0.0f }, {1.0f, 0.0f} }); // 右上

    vertexBuffer_.Create(vertices);

    SetColor({ 1.f, 1.f, 1.f, 1.f });
}

void Sprite::Update(void)
{
    TransferVertex();
    UpdateMatrix();
}

void Sprite::Draw(void)
{
    if (isInvisible_) return;

    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // 頂点バッファビューの設定コマンド
    iDXPtr->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBuffer_.GetVbView());

    // 定数バッファビュー(CBV)の設定コマンド
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDXPtr->GetCommandList()->SetGraphicsRootConstantBufferView(2, sCbMatOrthoGraphic_.GetBuffer()->GetGPUVirtualAddress());

    // SRVヒープの先頭にあるSRVをルートパラメータ0番に設定
    iDXPtr->GetCommandList()->SetGraphicsRootDescriptorTable(0, imagePtr_->srvGpuHandle_);

    // 描画コマンド
    iDXPtr->GetCommandList()->DrawInstanced((unsigned int)vertexBuffer_.GetVerticesNum(), 1, 0, 0); // 全ての頂点を使って描画
}

void Sprite::TransferVertex(void)
{
    // アンカーポイント
    float left{ (0.f - anchorPoint_.x) * size_.x * scale_.x };
    float right{ (1.f - anchorPoint_.x) * size_.x * scale_.x };
    float top{ (0.f - anchorPoint_.y) * size_.y * scale_.y };
    float bottom{ (1.f - anchorPoint_.y) * size_.y * scale_.y };

    // 反転
    if (isFlipX_) {
        left = -left;
        right = -right;
    }

    if (isFlipY_) {
        top = -top;
        bottom = -bottom;
    }

    // 頂点座標
    std::vector<VertexPosUv_t> vertices;
    vertices.emplace_back(VertexPosUv_t{ {   left, bottom, 0.0f }, {0.0f, 1.0f} }); // 左下
    vertices.emplace_back(VertexPosUv_t{ {   left,    top, 0.0f }, {0.0f, 0.0f} }); // 左上
    vertices.emplace_back(VertexPosUv_t{ {  right, bottom, 0.0f }, {1.0f, 1.0f} }); // 右下
    vertices.emplace_back(VertexPosUv_t{ {  right,    top, 0.0f }, {1.0f, 0.0f} }); // 右上

    // uv座標
    float texLeft{ cutStartPoint_.x / imagePtr_->buff_->GetDesc().Width };
    float texRight{ (cutStartPoint_.x + cutLength_.x) / imagePtr_->buff_->GetDesc().Width };
    float texTop{ cutStartPoint_.y / imagePtr_->buff_->GetDesc().Height };
    float texBottom{ (cutStartPoint_.y + cutLength_.y) / imagePtr_->buff_->GetDesc().Height };

    vertices[0].uv_ = { texLeft, texBottom };
    vertices[1].uv_ = { texLeft,    texTop };
    vertices[2].uv_ = { texRight, texBottom };
    vertices[3].uv_ = { texRight,    texTop };

    vertexBuffer_.TransferVertexToBuffer(vertices);
}

void Sprite::UpdateMatrix(void)
{
    matWorld_ = Math::Matrix::Identity();
    Matrix4 matRotation{ Math::Matrix::Identity() };

    matRotation *= Math::Matrix::RotationZ(rotation_);
    matWorld_ *= matRotation;

    matWorld_ = Math::Matrix::Translate(matWorld_, { position_.x,position_.y,0.f });

    if (parent_) matWorld_ *= parent_->matWorld_;

    // 定数バッファに転送
    cb_.GetConstBuffMap()->matWorld_ = matWorld_;
}

void Sprite::SetColor(Vector4 rgba)
{
    // 値が0.0fより小さい時0.0fにする
    rgba.x = (std::max)(rgba.x, 0.0f);
    rgba.y = (std::max)(rgba.y, 0.0f);
    rgba.z = (std::max)(rgba.z, 0.0f);
    rgba.w = (std::max)(rgba.w, 0.0f);

    // 値が1.0fより大きい時1.0fにする
    rgba.x = (std::min)(1.0f, rgba.x);
    rgba.y = (std::min)(1.0f, rgba.y);
    rgba.z = (std::min)(1.0f, rgba.z);
    rgba.w = (std::min)(1.0f, rgba.w);

    // 値を書き込むと自動的に転送される
    cb_.GetConstBuffMap()->color_ = rgba;
}

void Sprite::SetColor255(Vector4 rgba)
{
    // 値が0.0fより小さい時0.0fにする
    rgba.x = (std::max)(rgba.x, 0.0f);
    rgba.y = (std::max)(rgba.y, 0.0f);
    rgba.z = (std::max)(rgba.z, 0.0f);
    rgba.w = (std::max)(rgba.w, 0.0f);

    // 値が255.0fより大きい時255.0fにする
    rgba.x = (std::min)(255.0f, rgba.x);
    rgba.y = (std::min)(255.0f, rgba.y);
    rgba.z = (std::min)(255.0f, rgba.z);
    rgba.w = (std::min)(255.0f, rgba.w);

    // 正規化
    rgba = { rgba.x / 255.f, rgba.y / 255.f, rgba.z / 255.f, rgba.w / 255.f };

    // 値を書き込むと自動的に転送される
    cb_.GetConstBuffMap()->color_ = rgba;
}


