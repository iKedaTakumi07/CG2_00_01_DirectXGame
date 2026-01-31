#pragma once
#include <map>
#include <memory>
#include <string>

class DirectXCommon;
class Model;
class ModelCommon;

class ModelManager {
public:
    /// <summary>
    /// モデルファイルの読み込み
    /// </summary>
    /// <param name="filePath">モデルファイルのパス</param>
    void LoadModel(const std::string& filePath);

    /// <summary>
    /// モデルの検索
    /// </summary>
    /// <param name="filePath">モデルファイルのパス</param>
    /// <returns></returns>
    Model* FindModel(const std::string& filePath);

    // シングルトンインスタンスの取得
    static ModelManager* GetInstance();
    // 終了
    void Finalize();

    // 初期化
    void Initialize(DirectXCommon* dxCommon);

private:
    // モデルデータ
    std::map<std::string, std::unique_ptr<Model>> models;
    ModelCommon* modelCommon = nullptr;

    static ModelManager* instance;
    static bool finalized;

    ModelManager() = default;
    ~ModelManager() = default;

    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;
};
