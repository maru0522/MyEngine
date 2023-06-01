#include "Object3D.h"
#include "InitDirectX.h"

ModelManager* Object3D::modelMPtr_{ nullptr };
TextureManager* Object3D::texMPtr_{ nullptr };
CameraManager* Object3D::camMPtr_{ nullptr };
ConstBuffer<Object3D::CBMatViewPerse_t> Object3D::cbMatViewPerse_{};

void Object3D::StaticInitialize(ModelManager* modelMPtr, TextureManager* texMPtr, CameraManager* camMPtr)
{
    modelMPtr_ = modelMPtr;
    texMPtr_ = texMPtr;
    camMPtr_ = camMPtr;
    cbMatViewPerse_.Create();
    UpdateCBMatViewPerse();
}

void Object3D::UpdateCBMatViewPerse(void)
{
    cbMatViewPerse_.GetConstBuffMap()->matView_ = camMPtr_->GetCurrentCamera()->GetMatView();
    cbMatViewPerse_.GetConstBuffMap()->matPerspective_ = camMPtr_->GetCurrentCamera()->GetMatProjPerspective();
}

void Object3D::PreDraw(BlendMode blendmode)
{
    // インスタンス取得
    InitDirectX* iDX = InitDirectX::GetInstance();

    // プリミティブ形状の設定コマンド
    iDX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

    ID3D12DescriptorHeap* ppHeaps[] = { iDX->GetDescHeap_t()->GetDescHeap() };
    // SRVヒープの設定コマンド
    iDX->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    // パイプラインステートとルートシグネチャの設定コマンド
    iDX->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline3d(blendmode).pipelineState_.Get());
    iDX->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline3d(blendmode).rootSignature_.Get());
}

void Object3D::SetDrawBlendMode(BlendMode blendmode)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // パイプラインステートとルートシグネチャの設定コマンド
    iDXPtr->GetCommandList()->SetPipelineState(GraphicsPipeline::GetInstance()->GetPipeline3d(blendmode).pipelineState_.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(GraphicsPipeline::GetInstance()->GetPipeline3d(blendmode).rootSignature_.Get());
}

Object3D::Object3D(const fsPath& path) :
    parent_(nullptr)
{
    // モデル読み込み（値渡し）※出来れば、ptr渡しがいいんだけど、マテリアル変更しちゃうと大元が変わっちゃうからメッシュだけptr渡しして、マテリアルを別途持たせる構造にしたいんだけど現状メッシュとマテリアルは完全に癒着してるイメージだからやり方考えるべし
    model_ = modelMPtr_->GetModel(path);
    // 定数バッファ生成
    cb_.Create();

    // マテリアルを定数バッファへ転送
    model_.UpdateCB();
}

void Object3D::Update(void)
{
    // 座標計算と転送
    coordinate_.Update();
    cb_.GetConstBuffMap()->matWorld_ = coordinate_.GetMatWorld();
}

void Object3D::Draw(void)
{
    Draw(texMPtr_->GetImagePtr(model_.material_.texKey_)->srvGpuHandle_);
}

void Object3D::Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture)
{
    InitDirectX* iDX = InitDirectX::GetInstance();

    // 頂点バッファ
    iDX->GetCommandList()->IASetVertexBuffers(0, 1, &model_.mesh_.GetVBPtr()->GetVbView());
    // インデックスバッファ
    iDX->GetCommandList()->IASetIndexBuffer(&model_.mesh_.GetIBPtr()->GetIbView());

    // 定数バッファビュー（CBV）の設定コマンド
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(2, cbMatViewPerse_.GetBuffer()->GetGPUVirtualAddress());
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(3, model_.cbMaterial_.GetBuffer()->GetGPUVirtualAddress());

    //指定のヒープにあるSRVをルートパラメータ0番に設定
    iDX->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture);

    // 描画コマンドリスト
    iDX->GetCommandList()->DrawIndexedInstanced((uint32_t)model_.mesh_.GetIBPtr()->GetIndicesNum(), 1, 0, 0, 0);
}

void Object3D::Draw(const fsPath& path)
{
    Draw(texMPtr_->GetImagePtr(path)->srvGpuHandle_);
}
