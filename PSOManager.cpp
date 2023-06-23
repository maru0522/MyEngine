#include "PSOManager.h"
#include <cassert>
#include "Util.h"
#include "InitDirectX.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

void GraphicsPipeline::Create(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, size_t patternNum, Blob_t* blobsPtr, size_t  rootParamsCBNum, D3D12_CULL_MODE cullmode, BlendMode mode)
{
    //// デスクリプタレンジ
    //CD3DX12_DESCRIPTOR_RANGE cDescRange{};
    //cDescRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
    //// ルートパラメータ
    //std::vector<D3D12_ROOT_PARAMETER> rootParams{};
    //CD3DX12_ROOT_PARAMETER cRootParam1{};
    //cRootParam1.InitAsDescriptorTable(1, &cDescRange); // テクスチャレジスタ0番
    //rootParams.emplace_back(cRootParam1);

    //for (size_t i = 0; i < rootParamsCBNum; i++)
    //{
    //    CD3DX12_ROOT_PARAMETER cRootParamOfi{};
    //    cRootParamOfi.InitAsConstantBufferView((UINT)i); // 定数バッファ 0~i 番
    //    rootParams.emplace_back(cRootParamOfi);
    //}

    // ルートパラメータコンテナ
    std::vector<D3D12_ROOT_PARAMETER> rootParams{};

    for (size_t i = 0; i < 1; i++)
    {
        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE cDescRangeSRV{};
        cDescRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)i, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        // ルートパラメータ生成と追加 - DescTable
        CD3DX12_ROOT_PARAMETER cRootParamDT{};
        cRootParamDT.InitAsDescriptorTable(1, &cDescRangeSRV, D3D12_SHADER_VISIBILITY_ALL); // テクスチャレジスタ0番
        rootParams.emplace_back(cRootParamDT);
    }
    for (size_t i = 0; i < rootParamsCBNum; i++)
    {
        // ルートパラメータ生成と追加 - ConstantBufferView
        CD3DX12_ROOT_PARAMETER cRootParamCBV{};
        cRootParamCBV.InitAsConstantBufferView((UINT)i); // 定数バッファ 0~i 番
        rootParams.emplace_back(cRootParamCBV);
    }

    //// ルートパラメータコンテナ
    //std::vector<D3D12_ROOT_PARAMETER> rootParams{}; 
    //CreateRootParameter(rootParams,{ 1,rootParamsCBNum });

    //テクスチャサンプラーの変数宣言
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    // ルートシグネチャデスク
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = CreateRootSignatureDesc(samplerDesc, patternNum, rootParams);
    // シリアライズルートシグネチャオブジェクト
    ComPtr<ID3DBlob> rootSigBlob{ nullptr };
    // シリアライズルートシグネチャ
    VSerializeRootSignature(rootSigBlob.GetAddressOf(), &rootSignatureDesc);
    // ルートシグネチャ生成
    VCreateRootSignature(rootSigBlob.Get(), pipelineStateObject_.rootSignature.GetAddressOf());

    // パイプラインデスク
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = CreatePipelineDesc(blobsPtr, inputLayoutRef, pipelineStateObject_, cullmode, mode);
    // パイプラインステートオブジェクト生成
    VCreatePSO(&pipelineDesc, pipelineStateObject_.pipelineState.GetAddressOf());
}

void HelperGraphicPipeline::CreateRootParameter(std::vector<D3D12_ROOT_PARAMETER>& rootParams, RootParameterStructure_t rootParameterStructure)
{
    for (size_t i = 0; i < rootParameterStructure.descriptorRangeNum; i++)
    {
        // デスクリプタレンジ
        CD3DX12_DESCRIPTOR_RANGE cDescRangeSRV{};
        cDescRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)i, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        // ルートパラメータ生成と追加 - DescTable
        CD3DX12_ROOT_PARAMETER cRootParamDT{};
        cRootParamDT.InitAsDescriptorTable(1, &cDescRangeSRV, D3D12_SHADER_VISIBILITY_ALL); // テクスチャレジスタ0番
        rootParams.emplace_back(cRootParamDT);
    }
    for (size_t i = 0; i < rootParameterStructure.rootParamsCBNum; i++)
    {
        // ルートパラメータ生成と追加 - ConstantBufferView
        CD3DX12_ROOT_PARAMETER cRootParamCBV{};
        cRootParamCBV.InitAsConstantBufferView((UINT)i); // 定数バッファ 0~i 番
        rootParams.emplace_back(cRootParamCBV);
    }
}

D3D12_ROOT_SIGNATURE_DESC HelperGraphicPipeline::CreateRootSignatureDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, size_t patternNum, const std::vector<D3D12_ROOT_PARAMETER>& rootParamsRef)
{
    //テクスチャサンプラーの設定
    SetSamplerDesc(samplerDescRef, patternNum);

    // ルートシグネチャデスクの変数宣言
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    // ルートシグネチャデスクの設定
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParamsRef.data(); // ルートパラメータの先頭アドレス
    rootSignatureDesc.NumParameters = static_cast<uint32_t>(rootParamsRef.size());
    rootSignatureDesc.pStaticSamplers = &samplerDescRef;
    rootSignatureDesc.NumStaticSamplers = 1;

    return rootSignatureDesc;
}

void HelperGraphicPipeline::VSerializeRootSignature(ID3DBlob** rootSigBlobDPtr, D3D12_ROOT_SIGNATURE_DESC* rootSignatureDescPtr)
{
    // エラーオブジェクト
    ComPtr<ID3DBlob> errorBlob{ nullptr };
    HRESULT hr = D3D12SerializeRootSignature(rootSignatureDescPtr, D3D_ROOT_SIGNATURE_VERSION_1_0, rootSigBlobDPtr, &errorBlob);
    assert(SUCCEEDED(hr));
}

void HelperGraphicPipeline::VCreateRootSignature(ID3DBlob* rootSigBlobPtr, ID3D12RootSignature** rootSignatureDPtr)
{
    HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlobPtr->GetBufferPointer(), rootSigBlobPtr->GetBufferSize(), IID_PPV_ARGS(rootSignatureDPtr));
    assert(SUCCEEDED(hr));
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC HelperGraphicPipeline::CreatePipelineDesc(Blob_t* blobsPtr, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, const PSO_t& psoRef, D3D12_CULL_MODE cullmode, BlendMode mode)
{
    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

    // シェーダーの設定
    if (blobsPtr->vsBlob.Get()) { pipelineDesc.VS = CD3DX12_SHADER_BYTECODE(blobsPtr->vsBlob.Get()); }
    if (blobsPtr->gsBlob.Get()) { pipelineDesc.GS = CD3DX12_SHADER_BYTECODE(blobsPtr->gsBlob.Get()); }
    if (blobsPtr->psBlob.Get()) { pipelineDesc.PS = CD3DX12_SHADER_BYTECODE(blobsPtr->psBlob.Get()); }

    // サンプルマスクの設定
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

    // ラスタライザの設定
    switch (cullmode)
    {
    case D3D12_CULL_MODE_NONE:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
        break;
    case D3D12_CULL_MODE_FRONT:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT; // カリングしない
        break;
    case D3D12_CULL_MODE_BACK:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK; // カリングしない
        break;
    default:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
        break;
    }
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
    pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

    // ブレンドモード設定
    SetBlend(pipelineDesc, mode);

    // 頂点レイアウトの設定
    pipelineDesc.InputLayout.pInputElementDescs = inputLayoutRef.data();
    pipelineDesc.InputLayout.NumElements = (UINT)inputLayoutRef.size();

    // 図形の形状設定
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // その他の設定
    pipelineDesc.NumRenderTargets = 2; // 描画対象は1つ
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
    pipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
    pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

    // パイプラインにルートシグネチャをセット
    pipelineDesc.pRootSignature = psoRef.rootSignature.Get();

    return pipelineDesc;
}

void HelperGraphicPipeline::VCreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* pipelineDescPtr, ID3D12PipelineState** psoDPtr)
{
    // パイプライン設定を保存
    HRESULT hr = InitDirectX::GetInstance()->GetDevice()->CreateGraphicsPipelineState(pipelineDescPtr, IID_PPV_ARGS(psoDPtr));
    assert(SUCCEEDED(hr));
}

void HelperGraphicPipeline::SetSamplerDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, size_t patternNum)
{
    switch (patternNum)
    {
    case 1:
        //テクスチャサンプラーの設定
        samplerDescRef.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し(タイリング)
        samplerDescRef.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
        samplerDescRef.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
        samplerDescRef.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
        samplerDescRef.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てシニア補間
        samplerDescRef.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
        samplerDescRef.MinLOD = 0.0f;//ミップマップ最小値
        samplerDescRef.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDescRef.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
        break;
    case 2:
        //テクスチャサンプラーの設定
        samplerDescRef.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//横繰り返し(タイリング)
        samplerDescRef.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//縦繰り返し（タイリング）
        samplerDescRef.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
        samplerDescRef.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
        samplerDescRef.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        samplerDescRef.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
        samplerDescRef.MinLOD = 0.0f;//ミップマップ最小値
        samplerDescRef.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDescRef.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
        break;
    default:
        //テクスチャサンプラーの設定
        samplerDescRef.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し(タイリング)
        samplerDescRef.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
        samplerDescRef.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
        samplerDescRef.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
        samplerDescRef.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てシニア補間
        samplerDescRef.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
        samplerDescRef.MinLOD = 0.0f;//ミップマップ最小値
        samplerDescRef.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDescRef.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
        break;
    }
}

void HelperGraphicPipeline::SetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target)
{
    std::wstring wFilename{ Util::Convert::ToWString(filename) };

    // エラーオブジェクト
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob{ nullptr };

    // 頂点シェーダの読み込みとコンパイル
    HRESULT hr = D3DCompileFromFile(wFilename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), target.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, dp_blob, &errorBlob);

    // エラーなら
    if (FAILED(hr)) {
        // errorBlobからエラー内容をstring型にコピー
        std::string error;
        error.resize(errorBlob->GetBufferSize());
        std::copy_n((char*)errorBlob->GetBufferPointer(),
            errorBlob->GetBufferSize(),
            error.begin());
        error += "\n";
        // エラー内容を出力ウィンドウに表示
        OutputDebugStringA(error.c_str());
        assert(0);
    }
}

void HelperGraphicPipeline::SetBlend(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipelineDesc, BlendMode mode)
{
    // レンダーターゲットのブレンド設定（ブレンドステート）
    D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

    // 共通設定（アルファ値）
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;            // 加算
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;              // ソースの値を100%使う
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;            // デストの値を0%使う

    switch (mode)
    {
    case GraphicsPipeline::BlendMode::NONE:
        // ブレンドを無効にする
        blendDesc.BlendEnable = false;
        break;

    case GraphicsPipeline::BlendMode::ALPHA:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 半透明合成
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // ソースのアルファ値
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f - ソースのアルファ値
        break;

    case GraphicsPipeline::BlendMode::ADDITIVE:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 加算合成(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // ソースの値を100%使う
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // デストの値を100%使う
        break;

    case GraphicsPipeline::BlendMode::SUBTRACTIVE:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 減算合成(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // デストカラソースを減算
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // ソースの値を100%使う
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // デストの値を100%使う
        break;

    case GraphicsPipeline::BlendMode::MULTIPLICATION:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 減算合成(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        break;

    case GraphicsPipeline::BlendMode::INVERSION:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 色反転(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
        blendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;    // 1.0f - デストカラーの値
        blendDesc.DestBlend = D3D12_BLEND_ZERO;             // 使わない
        break;

    default:
        // ブレンドを無効にする
        blendDesc.BlendEnable = false;
        break;
    }

    pipelineDesc.BlendState.RenderTarget[1] = blendDesc;
}

PSOManager* PSOManager::GetInstance(void)
{
    static PSOManager sInstance{};
    return &sInstance;
}

void PSOManager::Create()
{
    // sprite
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout1; // xyz座標,uv座標
    inputLayout1.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    inputLayout1.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    std::unique_ptr<Blob_t> blobs1{ std::make_unique<Blob_t>() };
    VCompileShaderVS(blobs1.get(), "SpriteVS.hlsl", "main");
    VCompileShaderPS(blobs1.get(), "SpritePS.hlsl", "main");
    CreateAllBlendType(inputLayout1, 1, blobs1.get(), 2, D3D12_CULL_MODE::D3D12_CULL_MODE_NONE, "PSO_SPRITE");

    // object3D
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout2; // xyz座標,法線ベクトル,uv座標
    inputLayout2.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    inputLayout2.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 });
    inputLayout2.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    std::unique_ptr<Blob_t> blobs2{ std::make_unique<Blob_t>() };
    VCompileShaderVS(blobs2.get(), "ModelVS.hlsl", "main");
    VCompileShaderPS(blobs2.get(), "ModelPS.hlsl", "main");
    CreateAllBlendType(inputLayout2, 1, blobs2.get(), 4, D3D12_CULL_MODE::D3D12_CULL_MODE_BACK, "PSO_OBJECT3D");

    // postEffect
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout3; // xyz座標,uv座標
    inputLayout3.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    inputLayout3.emplace_back(D3D12_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    std::unique_ptr<Blob_t> blobs3{ std::make_unique<Blob_t>() };
    VCompileShaderVS(blobs3.get(), "PostEffectVS.hlsl", "main");
    VCompileShaderPS(blobs3.get(), "PostEffectPS.hlsl", "main");
    CreateAllBlendType(inputLayout3, 2, blobs3.get(), 1, D3D12_CULL_MODE::D3D12_CULL_MODE_NONE, "PSO_POSTEFFECT");
}

void PSOManager::CreateAllBlendType(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, size_t patternNum, Blob_t* blobsPtr, size_t  rootParamsCBNum, D3D12_CULL_MODE cullmode, const std::string& mapKey)
{
    std::array<GraphicsPipeline, static_cast<size_t>(HelperGraphicPipeline::BlendMode::MAX_ARRAY)> psoBlock;
    for (size_t i = 0; i < psoBlock.size(); i++)
    {
        psoBlock[i].Create(inputLayoutRef, patternNum, blobsPtr, rootParamsCBNum, cullmode, static_cast<BlendMode>(i));
    }

    pipelineStateObjects_.insert({ mapKey,psoBlock });
}
