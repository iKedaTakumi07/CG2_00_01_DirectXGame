#include "TextureManager.h"
#include "DirectXCommon.h"

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

void TextureManager::Initialize(DirectXCommon* DirectXCollision)
{
    this->textureManager_ = DirectXCollision;

    textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
    // テクスチャファイルを読み込んでプログラムで使えるようにする
    DirectX::ScratchImage image {};
    std::wstring filePathW = StringUtility::ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    // ミップマップの作成
    DirectX::ScratchImage mipImages {};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

    // テクスチャデータを追加
    textureDatas.resize(textureDatas.size() + 1);
    // 追加したテクスチャデータの参照を取得
    TextureData& textureData = textureDatas.back();

    textureData.filePath = filePath;
    textureData.metadata = image.GetMetadata();
    textureData.resource = textureManager_->CreateTextureResource(textureManager_->GetDevice(), textureData.metadata);

    // テクスチャデータの要素番号
    uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1);

    textureData.srvHandleCPU = textureManager_->GetSRVCPUDescriptorHandle(srvIndex);
    textureData.srvHandleGPU = textureManager_->GetSRVGPUDescriptorHandle(srvIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc {};
    srvDesc.Format = textureData.metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

    // SRVの生成
    textureManager_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);


    DirectXCommon::UploadTextureData
    textureManager_->UploadTextureData(textureData.resource.Get(), image);
}
