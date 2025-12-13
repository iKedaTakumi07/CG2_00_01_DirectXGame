#include "TextureManager.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::getInstance()
{
    if (instance == nullptr) {
        instance = new TextureManager;
    }
    return instance;
}

void TextureManager::Finalize()
{
    delete instance;
    instance = nullptr;
}

void TextureManager::Initialize(DirectXCommon* DirectXCollision, SrvManager* srvManager)
{
    this->dxCommon = DirectXCollision;
    this->srvManager = srvManager;

    textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
    if (textureDatas.contains(filePath)) {
        return;
    }

    // テクスチャファイルを読み込んでプログラムで使えるようにする
    DirectX::ScratchImage image {};
    std::wstring filePathW = StringUtility::ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImages {};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

    // テクスチャデータを追加
    // textureDatas.resize(textureDatas.size() + 1);
    // 追加したテクスチャデータの参照を取得
    TextureData& textureData = textureDatas[filePath];

    textureData.filePath = filePath;
    textureData.metadata = mipImages.GetMetadata();
    textureData.resource = dxCommon->CreateTextureResource(textureData.metadata);

    // テクスチャデータの要素数番号からSRVのインデクスを計算
    uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
    assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);

    textureData.srvIndex = srvManager->Allocate();
    textureData.srvHandleCPU = srvManager->GetCPUDescriptorHandle(textureData.srvIndex);
    textureData.srvHandleGPU = srvManager->GetGPUDescriptorHandle(textureData.srvIndex);

    // D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc {};
    // srvDesc.Format = textureData.metadata.format;
    // srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    // srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
    // dxCommon->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

    // SRVの生成
    srvManager->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));


    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon->UploadTextureData(textureData.resource, mipImages);

    /*UploadTextureData内に転送確定後に解放処理しているためいらない？*/
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
    // 読み込み済みテクスチャを検索

    if (textureDatas.contains(filePath)) {
        uint32_t textureIndex = GetSrvIndex(filePath);
        return textureIndex;
    }

    assert(0);
    return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandelGPU(const std::string& filePath)
{
    TextureData& textureData = textureDatas[filePath];
    return textureData.srvHandleGPU;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{
    return 0;
}

const DirectX::TexMetadata& TextureManager::GetMetadata(const std::string& filePath)
{
    TextureData& textureData = textureDatas[filePath];
    return textureData.metadata;
}
