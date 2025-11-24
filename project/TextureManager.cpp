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

void TextureManager::Finalize() {
    delete instance;
    instance = nullptr;
}

void TextureManager::Initialize() {
    textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}