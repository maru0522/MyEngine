#include "TextureManager.h"
#include "Util.h"
#include "Vector4.h"

void TextureManager::Initialize(InitDirectX* p_idx)
{
    p_idx_ = p_idx;

    GenerateMissingImage();
}

void TextureManager::Register(const Image& img)
{
    Register(img, "none", false);
}

void TextureManager::Register(const Image& img, const std::string& nickname, bool isAddNickname = true)
{
    if (isAddNickname) {
        // nicknameが登録済みかチェック
        if (paths_.count(nickname)) {
            // 登録済みの場合ログ出力
            Util::Log::PrintOutputWindow("[TextureManager] : This nickname is already in use. (" + nickname + ") The registration has therefore been cancelled.");

            // 登録は行わない。
            return;
        }

        // nicknameの要素構築
        paths_.emplace(nickname, img.path_);
    }

    // Imageが登録済みかチェック
    if (textures_.count(img.path_)) {
        // 登録済みの場合ログ出力
        Util::Log::PrintOutputWindow("[TextureManager] : This image is already loaded. (" + img.path_.string() + ")");

        // 登録は行わない。
        return;
    }

    // 要素構築
    textures_.emplace(img.path_, img);
}

void TextureManager::GenerateMissingImage(void)
{
#pragma region missingTexture生成
    Image tempImg{}; // てんぽらりん

    tempImg.path_ = "MISSING";

    // 一辺のピクセル数
    constexpr size_t imageLength{ 256 };
    // 配列の要素数
    constexpr size_t imageDataCount{ imageLength * imageLength };

    // イメージデータ配列
    Vector4* imageData{ new Vector4[imageDataCount] };

    // 生成
    for (size_t i = 0; i < imageDataCount; i++) {
        if (i < 32513) {
            if (i % 256 < 128) {
                imageData[i] = { 0.0f, 0.0f, 0.0f, 1.0f };
            }
            else if (i % 256 >= 128) {
                imageData[i] = { 1.0f, 0.0f, 1.0f, 1.0f };
            }
        }
        else {
            if (i % 256 < 128) {
                imageData[i] = { 1.0f, 0.0f, 1.0f, 1.0f };
            }
            else if (i % 256 >= 128) {
                imageData[i] = { 0.0f, 0.0f, 0.0f, 1.0f };
            }
        }
    }

#pragma endregion

#pragma region ヒープ設定とデスクリプタ設定
    // ヒープ設定
    D3D12_HEAP_PROPERTIES texHeapProp{};
    texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // リソース設定
    D3D12_RESOURCE_DESC textureResourceDesc{};
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureResourceDesc.Width = imageLength;
    textureResourceDesc.Height = imageLength;
    textureResourceDesc.DepthOrArraySize = 1;
    textureResourceDesc.MipLevels = 1;
    textureResourceDesc.SampleDesc.Count = 1;
#pragma endregion

#pragma region テクスチャバッファ
    // テクスチャバッファの生成
    HRESULT r = p_idx_->GetDevice()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&tempImg.buff_));

    assert(SUCCEEDED(r));
#pragma endregion

#pragma region バッファへのデータ転送
    // テクスチャバッファにデータ転送
    r = tempImg.buff_->WriteToSubresource(
            0,
            nullptr,		// 全領域へコピー
            imageData,	// 元データアドレス
            sizeof(DirectX::XMFLOAT4) * imageLength,    // 1ラインサイズ
            sizeof(DirectX::XMFLOAT4) * imageDataCount  // 全サイズ
        );

    //イメージデータ解放
    delete[] imageData;

    assert(SUCCEEDED(r));
#pragma endregion

    // InitDirectX内のDescriptorHeapに生成。
    // 返り値でgpuのアドレスを取得。
    tempImg.srvGpuHandle_.ptr = p_idx_->GetDescHeap()->CreateSRV(textureResourceDesc, tempImg.buff_.Get());

    // 登録
    Register(tempImg);
}
