#include "TextureManager.h"
#include "Util.h"
#include "Vector4.h"
#include <DirectXTex.h>
#include <cassert>

void TextureManager::Initialize(void)
{
    GenerateMissingImage();
}

void TextureManager::Load(const fsPath& path)
{
    InitDirectX* p_idx{ InitDirectX::GetInstance() };

    Image tempImg{}; // てんぽらりん

    tempImg.path = path;

    // Image重複確認
    if (CheckDuplicateTexture(tempImg)) return;

#pragma region テクスチャ読み込み
    //画像イメージデータ配列
    DirectX::TexMetadata metadata{};
    DirectX::ScratchImage scratchImg{};

    // WICテクスチャのロードに使う pathを文字列変換
    std::string strPath{ tempImg.path.string() };
    std::wstring wStrPath{ strPath.begin(), strPath.end() };
    const wchar_t* szFile{ wStrPath.c_str() };

    // WICテクスチャのロード
    HRESULT hr = LoadFromWICFile(szFile, DirectX::WIC_FLAGS_NONE, &metadata, scratchImg);
    if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) return;
    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region ミップマップ
    DirectX::ScratchImage mipChain{};

    // ミップマップ生成
    hr = GenerateMipMaps(scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
    if (SUCCEEDED(hr)) {
        scratchImg = std::move(mipChain);
        metadata = scratchImg.GetMetadata();
    }
#pragma endregion

    // 読み込んだディフューズテクスチャをSRGBとして扱う
    metadata.format = DirectX::MakeSRGB(metadata.format);

#pragma region ヒープ設定とデスクリプタ設定
    // ヒープ設定
    D3D12_HEAP_PROPERTIES texHeapProp{};
    texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
    texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    // リソース設定
    D3D12_RESOURCE_DESC textureResourceDesc{};
    textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDesc.Format = metadata.format;
    textureResourceDesc.Width = metadata.width;
    textureResourceDesc.Height = static_cast<UINT>(metadata.height);
    textureResourceDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
    textureResourceDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
    textureResourceDesc.SampleDesc.Count = 1;
#pragma endregion

#pragma region テクスチャバッファ
    // テクスチャバッファの生成
    hr = p_idx->GetDevice()->CreateCommittedResource(&texHeapProp, D3D12_HEAP_FLAG_NONE, &textureResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&tempImg.buff));
    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region バッファへのデータ転送
    // 全ミップマップについて
    for (size_t i = 0; i < metadata.mipLevels; i++) {
        // ミップマップレベルを指定してイメージを取得
        const DirectX::Image* img = scratchImg.GetImage(i, 0, 0);

        // テクスチャバッファにデータ転送
        hr = tempImg.buff->WriteToSubresource(
            static_cast<UINT>(i),
            nullptr,		// 全領域へコピー
            img->pixels,	// 元データアドレス
            static_cast<UINT>(img->rowPitch),	// 1ラインサイズ
            static_cast<UINT>(img->slicePitch)	// 全サイズ
        );
        assert(SUCCEEDED(hr));
    }
#pragma endregion

    // InitDirectX内のDescriptorHeapに生成。
    // 返り値でgpuのアドレスを取得。
    tempImg.srvGpuHandle.ptr = p_idx->GetDescHeap_t()->CreateSRV(textureResourceDesc, tempImg.buff.Get());

    // 登録
    RegisterImg(tempImg);
}

void TextureManager::Load(const fsPath& path, const std::string& nickname)
{
    // nicknameの重複確認
    if (CheckDuplicateNickname(nickname)) return;

    Load(path);

    //nickname登録
    RegisterNickname(path, nickname);
}

void TextureManager::LoadFolder(const fsPath& path)
{
    for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(path)) {
        std::filesystem::path fileName{ file.path().filename().string() };

        if (file.path().extension() == ".png" || file.path().extension() == ".jpg" || file.path().extension() == ".jpeg") {
            Load(path / fileName);
        }
    }
}

const TextureManager::Image* TextureManager::GetImagePtr(const fsPath& path) const
{
    try
    {
        return &textures_.at(path);
    }
    catch (const std::out_of_range&)
    {
        // エラーログ出力
        Util::Log::PrintOutputWindow("[TextureManager] : Couldn't find a texture corresponding to the argument path(" + path.string() + "), so replaced it with \"Missing Texture\". ");
        return &textures_.at("MISSING");
    }
}

const TextureManager::Image* TextureManager::GetImagePtrByNickname(const std::string& nickname)
{
    fsPath path{};

    try
    {
        path = paths_.at(nickname);
    }
    catch (const std::exception&)
    {
        // エラーログ出力
        Util::Log::PrintOutputWindow("[TextureManager] : Couldn't find a path corresponding to the argument nickname(" + nickname + "), so replaced it with \"MISSING\". ");
        path = "MISSING";
    }

    return GetImagePtr(path);
}

void TextureManager::RegisterImg(const Image& img)
{
    // Imageの要素構築
    textures_.emplace(img.path, img);
}

void TextureManager::RegisterNickname(const fsPath& path, const std::string& nickname)
{
    // nicknameの要素構築
    paths_.emplace(nickname, path);
}

void TextureManager::GenerateMissingImage(void)
{
    InitDirectX* p_idx{ InitDirectX::GetInstance() };

#pragma region missingTexture生成
    Image tempImg{}; // てんぽらりん

    tempImg.path = "MISSING";

    // 一辺のピクセル数
    constexpr size_t imageLength{ 256 };
    // 配列の要素数
    constexpr size_t imageDataCount{ imageLength * imageLength };

    // イメージデータ配列
    std::vector<Vector4> imageData{imageDataCount};

    // 生成
    for (size_t i = 0; i < imageDataCount; i++) {
        if (i < 32513) {
            if (i % 256 < 128) {
                imageData[i] = {0.0f, 0.0f, 0.0f, 1.0f};
            }
            else if (i % 256 >= 128) {
                //imageData[i] = { 1.f, 0.f, 1.f, 1.f };
                imageData[i] = { 1.f, 1.f, 1.f, 1.f };
            }
        }
        else {
            if (i % 256 < 128) {
                //imageData[i] = { 1.f, 0.f, 1.f, 1.f };
                imageData[i] = { 1.f, 1.f, 1.f, 1.f };
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
    HRESULT hr = p_idx->GetDevice()->CreateCommittedResource(
        &texHeapProp,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&tempImg.buff));

    assert(SUCCEEDED(hr));
#pragma endregion

#pragma region バッファへのデータ転送
    // テクスチャバッファにデータ転送
    hr = tempImg.buff->WriteToSubresource(
        0,
        nullptr,		// 全領域へコピー
        imageData.data(),	// 元データアドレス
        sizeof(Vector4) * imageLength,    // 1ラインサイズ
        sizeof(Vector4) * imageDataCount  // 全サイズ
    );

    assert(SUCCEEDED(hr));
#pragma endregion

    // InitDirectX内のDescriptorHeapに生成。
    // 返り値でgpuのアドレスを取得。
    tempImg.srvGpuHandle.ptr = p_idx->GetDescHeap_t()->CreateSRV(textureResourceDesc, tempImg.buff.Get());


    // 登録
    RegisterImg(tempImg);
}

bool TextureManager::CheckDuplicateTexture(const Image& img)
{
    // Imageが登録済みかチェック
    if (textures_.count(img.path)) {
        // 登録済みの場合ログ出力
        Util::Log::PrintOutputWindow("[TextureManager] : This image is already loaded. (" + img.path.string() + ")");

        return true;
    }
    return false;
}

bool TextureManager::CheckDuplicateNickname(const std::string& nickname)
{
    // nicknameが登録済みかチェック
    if (paths_.count(nickname)) {
        // 登録済みの場合ログ出力
        Util::Log::PrintOutputWindow("[TextureManager] : This nickname is already in use. (" + nickname + ") The registration has therefore been cancelled.");

        return true;
    }
    return false;
}
