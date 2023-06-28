#include "PSOManager.h"
#include <cassert>
#include "Util.h"
#include "InitDirectX.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

void IGraphicsPipeline::Create(IGraphicsPipelineStructure_t* igpsPtr, BlendMode blendMode)
{
    // ルートパラメータコンテナ
    std::vector<CD3DX12_ROOT_PARAMETER> cRootParams{};
    cRootParams.reserve(igpsPtr->rps.descriptorRangeNum + igpsPtr->rps.rootParamsCBNum);

    std::vector<CD3DX12_DESCRIPTOR_RANGE> cDescRangeSRVs{};
    cDescRangeSRVs.reserve(igpsPtr->rps.descriptorRangeNum);
    for (size_t i = 0; i < igpsPtr->rps.descriptorRangeNum; i++)
    {
        // デスクリプタレンジ
        cDescRangeSRVs.emplace_back();
        cDescRangeSRVs.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, (UINT)i, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        // ルートパラメータ生成と追加 - DescTable
        cRootParams.emplace_back();
        cRootParams.back().InitAsDescriptorTable(1, &cDescRangeSRVs.back(), D3D12_SHADER_VISIBILITY_ALL); // テクスチャレジスタ0番
    }
    for (size_t i = 0; i < igpsPtr->rps.rootParamsCBNum; i++)
    {
        // ルートパラメータ生成と追加 - ConstantBufferView
        cRootParams.emplace_back();
        cRootParams.back().InitAsConstantBufferView((UINT)i); // 定数バッファ 0~i 番
    }

    //テクスチャサンプラー
    D3D12_STATIC_SAMPLER_DESC samplerDesc = CreateSamplerDesc(igpsPtr->samplerType); //SamplerType type
    // ルートシグネチャデスク
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = CreateRootSignatureDesc(samplerDesc, cRootParams);
    // シリアライズルートシグネチャ
    ComPtr<ID3DBlob> rootSigBlob{ nullptr };
    VSerializeRootSignature(rootSigBlob.GetAddressOf(), &rootSignatureDesc);
    // ルートシグネチャ生成
    VCreateRootSignature(rootSigBlob.Get(), pipelineStateObject_.rootSignature.GetAddressOf());

    // パイプラインデスク
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = CreatePipelineDesc(igpsPtr->blobs.get(), igpsPtr->inputLayouts, pipelineStateObject_, igpsPtr->cullMode, blendMode);
    // パイプラインステートオブジェクト生成
    VCreatePSO(&pipelineDesc, pipelineStateObject_.pipelineState.GetAddressOf());
}

std::vector<D3D12_ROOT_PARAMETER> HelperGraphicPipeline::CreateRootParameter(RootParameterStructure_t rootParameterStructure)
{
    // ルートパラメータコンテナ
    std::vector<D3D12_ROOT_PARAMETER> rootParams{};

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
    return rootParams;
}

D3D12_STATIC_SAMPLER_DESC HelperGraphicPipeline::CreateSamplerDesc(SamplerType type)
{
    //テクスチャサンプラーの変数宣言
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};

    switch (static_cast<int>(type))
    {
    case 0:
        //テクスチャサンプラーの設定
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し(タイリング)
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
        samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てシニア補間
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
        samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
        break;
    case 1:
        //テクスチャサンプラーの設定
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//横繰り返し(タイリング)
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;//縦繰り返し（タイリング）
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
        samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
        samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
        break;
    default:
        //テクスチャサンプラーの設定
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し(タイリング)
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
        samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てシニア補間
        samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
        samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
        break;
    }

    return samplerDesc;
}

D3D12_ROOT_SIGNATURE_DESC HelperGraphicPipeline::CreateRootSignatureDesc(D3D12_STATIC_SAMPLER_DESC& samplerDescRef, const std::vector<CD3DX12_ROOT_PARAMETER>& cRootParamsRef)
{
    // ルートシグネチャデスクの変数宣言
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    // ルートシグネチャデスクの設定
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = cRootParamsRef.data(); // ルートパラメータの先頭アドレス
    rootSignatureDesc.NumParameters = static_cast<uint32_t>(cRootParamsRef.size());
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

D3D12_GRAPHICS_PIPELINE_STATE_DESC HelperGraphicPipeline::CreatePipelineDesc(Blob_t* blobsPtr, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayoutRef, const PSO_t& psoRef, D3D12_CULL_MODE cullMode, BlendMode blendMode)
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
    switch (cullMode)
    {
    case D3D12_CULL_MODE_NONE:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
        break;
    case D3D12_CULL_MODE_FRONT:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
        break;
    case D3D12_CULL_MODE_BACK:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        break;
    default:
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
        break;
    }
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
    pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

    // ブレンドモード設定
    SetBlend(pipelineDesc, blendMode);

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

void HelperGraphicPipeline::VSetCompileShader(ID3DBlob** dp_blob, const std::string& filename, const std::string& entryPoint, const std::string& target)
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
    D3D12_RENDER_TARGET_BLEND_DESC blendDesc{};
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画

    // 共通設定（アルファ値）
    blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;            // 加算
    blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;              // ソースの値を100%使う
    blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;            // デストの値を0%使う

    switch (mode)
    {
    case IGraphicsPipeline::BlendMode::NONE:
        // ブレンドを無効にする
        blendDesc.BlendEnable = false;
        break;

    case IGraphicsPipeline::BlendMode::ALPHA:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 半透明合成
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;         // ソースのアルファ値
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;    // 1.0f - ソースのアルファ値
        break;

    case IGraphicsPipeline::BlendMode::ADDITIVE:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 加算合成(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;             // 加算
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // ソースの値を100%使う
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // デストの値を100%使う
        break;

    case IGraphicsPipeline::BlendMode::SUBTRACTIVE:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 減算合成(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;    // デストカラソースを減算
        blendDesc.SrcBlend = D3D12_BLEND_ONE;               // ソースの値を100%使う
        blendDesc.DestBlend = D3D12_BLEND_ONE;              // デストの値を100%使う
        break;

    case IGraphicsPipeline::BlendMode::MULTIPLICATION:
        // ブレンドを有効にする
        blendDesc.BlendEnable = true;
        // 減算合成(RGB値)
        blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
        blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
        blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        break;

    case IGraphicsPipeline::BlendMode::INVERSION:
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

    pipelineDesc.BlendState.RenderTarget[0] = blendDesc;
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
    AddPipeline("PSO_SPRITE");
    GetPSOBlendPtr("PSO_SPRITE")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_SPRITE")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_SPRITE")->CompileShader(ShaderType::VS, "SpriteVS.hlsl", "main");
    GetPSOBlendPtr("PSO_SPRITE")->CompileShader(ShaderType::PS, "SpritePS.hlsl", "main");
    GetPSOBlendPtr("PSO_SPRITE")->SetRootParameterStructure(1, 2);
    GetPSOBlendPtr("PSO_SPRITE")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("PSO_SPRITE")->SetSamplerType(SamplerType::NORMAL);
    SetComplete("PSO_SPRITE");

    // object3D
    AddPipeline("PSO_OBJECT3D");
    GetPSOBlendPtr("PSO_OBJECT3D")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_OBJECT3D")->AddInputLayout({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 });
    GetPSOBlendPtr("PSO_OBJECT3D")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_OBJECT3D")->CompileShader(ShaderType::VS, "ModelVS.hlsl", "main");
    GetPSOBlendPtr("PSO_OBJECT3D")->CompileShader(ShaderType::PS, "ModelPS.hlsl", "main");
    GetPSOBlendPtr("PSO_OBJECT3D")->SetRootParameterStructure(1, 4);
    GetPSOBlendPtr("PSO_OBJECT3D")->SetCullMode(D3D12_CULL_MODE_BACK);
    GetPSOBlendPtr("PSO_OBJECT3D")->SetSamplerType(SamplerType::NORMAL);
    SetComplete("PSO_OBJECT3D");

    // postEffect
    AddPipeline("PSO_POSTEFFECT");
    GetPSOBlendPtr("PSO_POSTEFFECT")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_POSTEFFECT")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("PSO_POSTEFFECT")->CompileShader(ShaderType::VS, "PostEffectVS.hlsl", "main");
    GetPSOBlendPtr("PSO_POSTEFFECT")->CompileShader(ShaderType::PS, "PostEffectPS.hlsl", "main");
    GetPSOBlendPtr("PSO_POSTEFFECT")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("PSO_POSTEFFECT")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("PSO_POSTEFFECT")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("PSO_POSTEFFECT");

    // gaussianblur
    AddPipeline("GAUSSIAN");
    GetPSOBlendPtr("GAUSSIAN")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("GAUSSIAN")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("GAUSSIAN")->CompileShader(ShaderType::VS, "GaussianBlurVS.hlsl", "main");
    GetPSOBlendPtr("GAUSSIAN")->CompileShader(ShaderType::PS, "GaussianBlurPS.hlsl", "main");
    GetPSOBlendPtr("GAUSSIAN")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("GAUSSIAN")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("GAUSSIAN")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("GAUSSIAN");

    // extractHiLumi
    AddPipeline("HIGHLUMI");
    GetPSOBlendPtr("HIGHLUMI")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("HIGHLUMI")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("HIGHLUMI")->CompileShader(ShaderType::VS, "HighLumiVS.hlsl", "main");
    GetPSOBlendPtr("HIGHLUMI")->CompileShader(ShaderType::PS, "HighLumiPS.hlsl", "main");
    GetPSOBlendPtr("HIGHLUMI")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("HIGHLUMI")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("HIGHLUMI")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("HIGHLUMI");

    // bloom
    AddPipeline("BLOOM");
    GetPSOBlendPtr("BLOOM")->AddInputLayout({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("BLOOM")->AddInputLayout({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
    GetPSOBlendPtr("BLOOM")->CompileShader(ShaderType::VS, "BloomVS.hlsl", "main");
    GetPSOBlendPtr("BLOOM")->CompileShader(ShaderType::PS, "BloomPS.hlsl", "main");
    GetPSOBlendPtr("BLOOM")->SetRootParameterStructure(2, 1);
    GetPSOBlendPtr("BLOOM")->SetCullMode(D3D12_CULL_MODE_NONE);
    GetPSOBlendPtr("BLOOM")->SetSamplerType(SamplerType::POSTEFFECT);
    SetComplete("BLOOM");
}

void PSOManager::GraphicsPipelineBlend_t::Create(void)
{
    for (size_t i = 0; i < static_cast<size_t>(BlendMode::MAX_ARRAY); i++)
    {
        blends[i].Create(&igps, static_cast<BlendMode>(i));
    }
}

void PSOManager::GraphicsPipelineBlend_t::CompileShader(ShaderType shaderType, const std::string& filename, const std::string& entryPoint)
{
    if (shaderType == ShaderType::VS) VSetCompileShader(igps.blobs->vsBlob.GetAddressOf(), filename, entryPoint, "vs_5_0");
    if (shaderType == ShaderType::GS) VSetCompileShader(igps.blobs->gsBlob.GetAddressOf(), filename, entryPoint, "gs_5_0");
    if (shaderType == ShaderType::PS) VSetCompileShader(igps.blobs->psBlob.GetAddressOf(), filename, entryPoint, "ps_5_0");
}

void PSOManager::GraphicsPipelineBlend_t::SetRootParameterStructure(size_t descriptorRangeNum, size_t rootParamsCBNum)
{
    igps.rps.descriptorRangeNum = descriptorRangeNum;
    igps.rps.rootParamsCBNum = rootParamsCBNum;
}
