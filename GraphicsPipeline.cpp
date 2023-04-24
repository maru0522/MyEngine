#include "GraphicsPipeline.h"
#include <cassert>
#include "Util.h"
#include "InitDirectX.h"
#include<d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")

GraphicsPipeline* GraphicsPipeline::GetInstance(void)
{
    static GraphicsPipeline instance{};

    return &instance;
}

const GraphicsPipeline::Pipeline_t& GraphicsPipeline::GetPipeline2d(BlendMode mode) const
{
    return pipelineProducts2d_.at(static_cast<int32_t>(mode));
}

void GraphicsPipeline::Initialize(void)
{
    for (size_t i = 0; i < pipelineProducts2d_.size(); i++) {
        Pipeline_t pipeline{};
        HelperGraphicPipeline::Pipeline2d(pipeline, static_cast<BlendMode>(i));

        pipelineProducts2d_.at(i) = pipeline;
    }

    //for (size_t i = 0; i < pipelineProducts3d_.size(); i++) {
    //    Pipeline_t pipeline{};
    //    HelperGraphicPipeline::Pipeline3d(pipeline, static_cast<BlendMode>(i));

    //    pipelineProducts3d_.at(i) = pipeline;
    //}
}

void HelperGraphicPipeline::Pipeline2d(Pipeline_t& pipeline, BlendMode mode)
{
    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        {   // xyz座標
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        { // uv座標
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
    };

    // blob宣言
    std::unique_ptr<Blob_t> blobs{ std::make_unique<Blob_t>() };
    ComPtr<ID3DBlob> errorBlob{ nullptr }; // エラーオブジェクト

    // 頂点シェーダの読み込みとコンパイル
    SetCompileShader(blobs->vsBlob.GetAddressOf(), "SpriteVS.hlsl", "main", "vs_5_0");

    // ピクセルシェーダの読み込みとコンパイル
    SetCompileShader(blobs->psBlob.GetAddressOf(), "SpritePS.hlsl", "main", "ps_5_0");

#pragma region パイプラインの設定
    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

    // シェーダーの設定
    pipelineDesc.VS.pShaderBytecode = blobs->vsBlob->GetBufferPointer();
    pipelineDesc.VS.BytecodeLength = blobs->vsBlob->GetBufferSize();
    pipelineDesc.PS.pShaderBytecode = blobs->psBlob->GetBufferPointer();
    pipelineDesc.PS.BytecodeLength = blobs->psBlob->GetBufferSize();

    // サンプルマスクの設定
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

    // ラスタライザの設定
    pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ポリゴン内塗りつぶし
    pipelineDesc.RasterizerState.DepthClipEnable = true; // 深度クリッピングを有効に

#pragma region ブレンド設定
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
#pragma endregion

    // 頂点レイアウトの設定
    pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
    pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

    // 図形の形状設定
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // その他の設定
    pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
    pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

#pragma region デスクリプタレンジ
    // デスクリプタレンジの変数宣言
    D3D12_DESCRIPTOR_RANGE descriptorRange{};
    // デスクリプタレンジの設定
    descriptorRange.NumDescriptors = 1;
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange.BaseShaderRegister = 0; // テクスチャレジスタ0番
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

#pragma region ルートパラメータ
    // ルートパラメーターの設定
    D3D12_ROOT_PARAMETER rootParams[2] = {};
    // テクスチャレジスタ0番
    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// 種類
    rootParams[0].DescriptorTable.pDescriptorRanges = &descriptorRange;
    rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    // 定数バッファ0番
    rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // 定数バッファビュー
    rootParams[1].Descriptor.ShaderRegister = 0;                    // 定数バッファ番号
    rootParams[1].Descriptor.RegisterSpace = 0;                     // デフォルト値
    rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   // 全てのシェーダから見える
    ////// 定数バッファ1番
    //rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // 定数バッファビュー
    //rootParams[2].Descriptor.ShaderRegister = 1;                    // 定数バッファ番号
    //rootParams[2].Descriptor.RegisterSpace = 0;                     // デフォルト値
    //rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;   // 全てのシェーダから見える
#pragma endregion

#pragma region サンプラーの設定
    //テクスチャサンプラーの変数宣言
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
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
#pragma endregion

#pragma region ルートシグネチャ
    // ルートシグネチャの変数宣言
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    // ルートシグネチャの設定
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParams; // ルートパラメータの先頭アドレス
    rootSignatureDesc.NumParameters = static_cast<uint32_t>(std::size(rootParams));
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;

    ComPtr<ID3DBlob> rootSigBlob{ nullptr }; // ルートシグネチャのシリアライズ

    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
    assert(SUCCEEDED(hr));

    hr = InitDirectX::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline.rootSignature_));
    assert(SUCCEEDED(hr));
#pragma endregion

    // パイプラインにルートシグネチャをセット
    pipelineDesc.pRootSignature = pipeline.rootSignature_.Get();
#pragma endregion パイプラインの設定

    // パイプライン設定を保存
    hr = InitDirectX::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipeline.pipelineState_));
    assert(SUCCEEDED(hr));
}

void HelperGraphicPipeline::Pipeline3d(Pipeline_t& pipeline, BlendMode mode)
{
    // 頂点レイアウト
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { // xyz座標(1行で書いたほうが見やすい)
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        { // 法線ベクトル（1行書いたほうがわかりやすい)
            "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
        },
        { // uv座標(1行で書いたほうが見やすい)
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
    };

    // blob宣言
    std::unique_ptr<Blob_t> blobs{ std::make_unique<Blob_t>() };
    ComPtr<ID3DBlob> errorBlob{ nullptr }; // エラーオブジェクト

    // 頂点シェーダの読み込みとコンパイル
    SetCompileShader(blobs->vsBlob.GetAddressOf(), "Resources/Shaders/ModelVS.hlsl", "main", "vs_5_0");

    // ピクセルシェーダの読み込みとコンパイル
    SetCompileShader(blobs->psBlob.GetAddressOf(), "Resources/Shaders/ModelPS.hlsl", "main", "ps_5_0");

#pragma region パイプラインの設定
    // グラフィックスパイプライン設定
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

    // シェーダーの設定
    pipelineDesc.VS.pShaderBytecode = blobs->vsBlob->GetBufferPointer();
    pipelineDesc.VS.BytecodeLength = blobs->vsBlob->GetBufferSize();
    pipelineDesc.PS.pShaderBytecode = blobs->psBlob->GetBufferPointer();
    pipelineDesc.PS.BytecodeLength = blobs->psBlob->GetBufferSize();

    // サンプルマスクの設定
    pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定

    // ラスタライザの設定
    pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;   // 背面をカリング
    pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;  // ポリゴン内塗りつぶし
    pipelineDesc.RasterizerState.DepthClipEnable = true;            // 深度クリッピングを有効に

#pragma region ブレンド設定
    // レンダーターゲットのブレンド設定（ブレンドステート）※これメンバにした方がいいんじゃね？？
    D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = pipelineDesc.BlendState.RenderTarget[0];
    blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画

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
#pragma endregion

    // 頂点レイアウトの設定
    pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
    pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

    // 図形の形状設定
    pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // その他の設定
    pipelineDesc.NumRenderTargets = 1; // 描画対象は1つ
    pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~255指定のRGBA
    pipelineDesc.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

#pragma region デスクリプタレンジ
    // デスクリプタレンジの設定
    D3D12_DESCRIPTOR_RANGE descriptorRange{};
    descriptorRange.NumDescriptors = 1;
    descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange.BaseShaderRegister = 0;	// テクスチャレジスタ0番
    descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
#pragma endregion

#pragma region ルートパラメータ
    // ルートパラメーターの設定
    D3D12_ROOT_PARAMETER rootParams[3] = {};
    // 定数バッファ0番
    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// 定数バッファビュー
    rootParams[0].Descriptor.ShaderRegister = 0;					// 定数バッファ番号
    rootParams[0].Descriptor.RegisterSpace = 0;						// デフォルト値
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	// 全てのシェーダから見える
    // 定数バッファ1番
    rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// 種類
    rootParams[1].Descriptor.ShaderRegister = 1;					// 定数バッファ番号
    rootParams[1].Descriptor.RegisterSpace = 0;						// デフォルト値
    rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	// 全てのシェーダから見える
    // テクスチャレジスタ0番
    rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// 種類
    rootParams[2].DescriptorTable.pDescriptorRanges = &descriptorRange;
    rootParams[2].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
#pragma endregion

#pragma region サンプラーの設定
    //テクスチャサンプラーの設定
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//横繰り返し(タイリング)
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//縦繰り返し（タイリング）
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//奥行繰り返し（タイリング）
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//ボーダーの時は黒
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//全てシニア補間
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//ミップマップ最大値
    samplerDesc.MinLOD = 0.0f;//ミップマップ最小値
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダからのみ使用可能
#pragma endregion

#pragma region ルートシグネチャ
    //ルートシグネチャの設定
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.pParameters = rootParams;//ルートパラメータの先頭アドレス
    rootSignatureDesc.NumParameters = _countof(rootParams);//ルートパラメータ数
    rootSignatureDesc.pStaticSamplers = &samplerDesc;
    rootSignatureDesc.NumStaticSamplers = 1;

    ComPtr<ID3DBlob> rootSigBlob{ nullptr }; // ルートシグネチャのシリアライズ
    HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
    assert(SUCCEEDED(hr));

    hr = InitDirectX::GetInstance()->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&pipeline.rootSignature_));
    assert(SUCCEEDED(hr));
#pragma endregion

    // パイプラインにルートシグネチャをセット
    pipelineDesc.pRootSignature = pipeline.rootSignature_.Get();

#pragma region 深度設定
    // デプスステンシルステートの設定
    pipelineDesc.DepthStencilState.DepthEnable = true;  // 深度テストを行う
    pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;     // 書き込み許可
    pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;          // 小さければ合格
    pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;                                 // 深度値フォーマット
#pragma endregion
#pragma endregion

#pragma region パイプラインステートの生成
    // パイプライン設定の保存
    hr = InitDirectX::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipeline.pipelineState_));
    assert(SUCCEEDED(hr));
#pragma endregion
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
