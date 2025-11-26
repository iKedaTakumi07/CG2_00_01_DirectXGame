#pragma once
#include "StringUtility.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class DirectXCommon;

class TextureManager {
public:
    static TextureManager* getInstance();

    void Finalize();

    // 初期化
    void Initialize(DirectXCommon* DirectXCollision);

    // テクスチャファイルの読み込み
    void LoadTexture(const std::string& filePath);

    // SRVインデクスの開始番号
    uint32_t GetTextureIndexByFilePath(const std::string& filePath);
    // テクスチャ番号からGPUハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandelGPU(uint32_t textureIndex);

    const DirectX::TexMetadata& GetMetadata(uint32_t textureIndex);

private:
    static TextureManager* instance;

    TextureManager() = default;
    ~TextureManager() = default;
    TextureManager(TextureManager&) = delete;
    TextureManager& operator=(TextureManager&) = delete;

    // テクスチャデータ
    struct TextureData {
        std::string filePath;
        DirectX::TexMetadata metadata;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
    };

    // テクスチャデータ
    std::vector<TextureData> textureDatas;

    DirectXCommon* dxCommon;

    static uint32_t kSRVIndexTop;
};
