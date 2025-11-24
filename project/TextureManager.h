#pragma once
#include "externals/DirectXTex/DirectXTex.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>
class DirectXCollision;

class TextureManager {
public:
    static TextureManager* getInstance();

    void Finalize();

    // 初期化
    void Initialize();

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
};
