#include "Object3D.h"
#include "InitDirectX.h"

LightManager* Object3D::sLightGroupPtr_{ nullptr };
ModelManager* Object3D::sModelMPtr_{ nullptr };
TextureManager* Object3D::sTexMPtr_{ nullptr };
ConstBuffer<Object3D::CBMatViewPerse_t> Object3D::sCbMatViewPerse_{};

void Object3D::StaticInitialize(ModelManager* modelMPtr, TextureManager* texMPtr)
{
    sModelMPtr_ = modelMPtr;
    sTexMPtr_ = texMPtr;
    sCbMatViewPerse_.Create();

    // 初期化時はカメラが存在しないためCameraManagerのSetCurrentCamera()時に実行するようにする
    //UpdateCBMatViewPerse();
}

void Object3D::UpdateCBMatViewPerse(CameraManager* camMPtr)
{
    // nullチェック
    if (!camMPtr) { return; }
    if (!camMPtr->GetCurrentCamera()) { return; }

    sCbMatViewPerse_.GetConstBuffMap()->matView = camMPtr->GetCurrentCamera()->GetMatView();
    sCbMatViewPerse_.GetConstBuffMap()->matPerspective = camMPtr->GetCurrentCamera()->GetMatProjPerspective();

    sCbMatViewPerse_.GetConstBuffMap()->cameraPos = camMPtr->GetCurrentCamera()->GetTransformPtr()->position;
}

void Object3D::PreDraw(BlendMode blendmode)
{
    // インスタンス取得
    InitDirectX* iDX = InitDirectX::GetInstance();

    // プリミティブ形状の設定コマンド
    iDX->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形リスト

    std::vector<ID3D12DescriptorHeap*> ppHeaps = { iDX->GetDescHeap_t()->GetDescHeap() };
    // SRVヒープの設定コマンド
    iDX->GetCommandList()->SetDescriptorHeaps((UINT)ppHeaps.size(), ppHeaps.data());

    // パイプラインステートとルートシグネチャの設定コマンド
    iDX->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D",blendmode)->pipelineState.Get());
    iDX->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D", blendmode)->rootSignature.Get());
}

void Object3D::SetDrawBlendMode(BlendMode blendmode)
{
    InitDirectX* iDXPtr = InitDirectX::GetInstance();

    // パイプラインステートとルートシグネチャの設定コマンド
    iDXPtr->GetCommandList()->SetPipelineState(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D", blendmode)->pipelineState.Get());
    iDXPtr->GetCommandList()->SetGraphicsRootSignature(PSOManager::GetInstance()->GetPSOPtr("PSO_OBJECT3D", blendmode)->rootSignature.Get());
}

Object3D::Object3D(const fsPath& path) :
    parent_(nullptr)
{
    // モデル読み込み
    model_ = sModelMPtr_->GetModel(path);
    // モデルのマテリアル用定数バッファを生成
    model_.cbMaterial_.Create();
    // マテリアルを定数バッファへ転送
    model_.UpdateCB();

    // 定数バッファ生成
    cb_.Create();
}

void Object3D::Update(void)
{
    // 座標計算と転送
    cb_.GetConstBuffMap()->matWorld = matTrans_.mat_world;
    model_.UpdateCB();
}

void Object3D::Draw(void)
{
    Draw(sTexMPtr_->GetImagePtr(model_.material_.texKey)->srvGpuHandle);
}

void Object3D::Draw(const D3D12_GPU_DESCRIPTOR_HANDLE& texture)
{
    if (isInvisible_) return;

    InitDirectX* iDX = InitDirectX::GetInstance();

    // 頂点バッファ
    iDX->GetCommandList()->IASetVertexBuffers(0, 1, &model_.meshPtr_->GetVBPtr()->GetVbView());
    // インデックスバッファ
    iDX->GetCommandList()->IASetIndexBuffer(&model_.meshPtr_->GetIBPtr()->GetIbView());

    // 定数バッファビュー（CBV）の設定コマンド
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(1, cb_.GetBuffer()->GetGPUVirtualAddress());
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(2, sCbMatViewPerse_.GetBuffer()->GetGPUVirtualAddress());
    iDX->GetCommandList()->SetGraphicsRootConstantBufferView(3, model_.cbMaterial_.GetBuffer()->GetGPUVirtualAddress());

    //指定のヒープにあるSRVをルートパラメータ0番に設定
    iDX->GetCommandList()->SetGraphicsRootDescriptorTable(0, texture);

    // 描画コマンドリスト
    iDX->GetCommandList()->DrawIndexedInstanced((uint32_t)model_.meshPtr_->GetIBPtr()->GetIndicesNum(), 1, 0, 0, 0);
}

void Object3D::Draw(const fsPath& path)
{
    Draw(sTexMPtr_->GetImagePtr(path)->srvGpuHandle);
}
