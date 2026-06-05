#include "TitleScene.h"
#include "SceneManager.h"

#include "../base/PostProcess.h"

#include <random>

#include "../3d/Camera.h"
#include "../base/WinApp.h"

#include "../2d/SpriteCommon.h"
#include "../base/TextureManager.h"

#include "../3d/Model.h"
#include "../3d/ModelManager.h"
#include "../3d/Object3d.h"
#include "../3d/Object3dCommon.h"

#include "../3d/Skybox/SkyBoxCommon.h"
#include "../3d/Skybox/Skybox.h"

#include "../3d/ParticleEmitter.h"
#include "../3d/ParticleManager.h"

#include "../io/Input.h"

#include "math.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene() = default;

void TitleScene::Initialize()
{
    TextureManager::getInstance()->LoadTexture("resources/rostock_laage_airport_4k.dds");
    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    TextureManager::getInstance()->LoadTexture("resources/grass.png");

    ModelManager::GetInstance()->LoadModel("axis.obj");
    ModelManager::GetInstance()->LoadModel("terrain.obj");

    skydox = std::make_unique<Skybox>();
    skydox->Initialize("resources/rostock_laage_airport_4k.dds");

    object3d = std::make_unique<Object3d>();
    object3d->Initialize();
    object3d->SetCamera(BaseScene::GetCamera());

    model = std::make_unique<Model>();
    model->Initialize("resources", "terrain.obj");
    model->SetEvnTexturefilePath(skydox->GetTextureFilePath());
    object3d->SetModel(model.get());

    ParticleManager::getInstance()->CreateParticleGroup("pori", "resources/circle2.png", ParticleMeshType::kPlane);
    ParticleManager::getInstance()->CreateParticleGroup("Plane", "resources/uvChecker.png", ParticleMeshType::kPlane);
    ParticleManager::getInstance()->CreateParticleGroup("gradationLine", "resources/gradationLine.png", ParticleMeshType::kRing);
    ParticleManager::getInstance()->CreateParticleGroup("Cylinder", "resources/gradationLine.png", ParticleMeshType::kCylinder);

    // ポストエフェクトのON/OFFならこれ。
    // [次回アップデート]BoxFilter,gaussianFilterのhlslを1つにするためKernelSizeで設定し定数バッファで渡してポストエフェクトを入れる、のを導入予定。
    PostProcess::GetInstance()->SetEnableGaussianFilter5x5(true);

    // 板ポリ
    Transform emitter {};
    emitter.translate = { 0.0f, 2.0f, 0.0f };
    emitter.rotate = { 0.0f, 0.0f, 1.0f };
    emitter.scale = { 0.05f, 1.0f, 1.0f };
    particleEmitter = std::make_unique<ParticleEmitter>("pori", emitter, 1.0f, 8, true);
    EmitterParam fireParam;
    fireParam.maxRotate = { 0.0f, 0.0f, std::numbers::pi_v<float> };
    fireParam.minRotate = { 0.0f, 0.0f, -std::numbers::pi_v<float> };
    fireParam.maxScale = { 0.05f, 1.5f, 1.0f };
    fireParam.minScale = { 0.05f, 0.4f, 1.0f };
    fireParam.SetStartColor({ 1.0f, 0.0f, 1.0f, 1.0f });
    fireParam.SetEndColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    fireParam.SetVelocity({ 0.0f, 0.0f, 0.0f });
    fireParam.SetLifeTime(1.0f);
    particleEmitter->SetParam(fireParam);

    particleEmitter2 = std::make_unique<ParticleEmitter>("gradationLine", emitter, 1.0f, 3, true);
    fireParam.maxRotate = { std::numbers::pi_v<float>, std::numbers::pi_v<float>, 0.0f };
    fireParam.minRotate = { -std::numbers::pi_v<float>, -std::numbers::pi_v<float>, 0.0f };
    fireParam.maxScale = { 1.0f, 1.0f, 1.0f };
    fireParam.minScale = { 1.0f, 0.4f, 1.0f };
    fireParam.SetStartColor({ 0.0f, 1.0f, 1.0f, 1.0f });
    fireParam.SetEndColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    fireParam.SetVelocity({ 0.0f, 0.0f, 0.0f });
    fireParam.SetLifeTime(1.0f);
    particleEmitter2->SetParam(fireParam);

    // [アップデート予定]パーティクルPS閾値をCBuffer経由で設定できるようにする
    emitter.translate = { 0.0f, 0.0f, 0.0f };
    particleEmitter3 = std::make_unique<ParticleEmitter>("Cylinder", emitter, 10.0f, 1, false);
    fireParam.SetScale({ 1.0f, 1.0f, 1.0f });
    fireParam.SetRotate({ 0.0f, 0.0f, 0.0f });
    fireParam.SetStartColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    fireParam.SetEndColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    fireParam.SetVelocity({ 0.0f, 0.0f, 0.0f });
    fireParam.SetLifeTime(10.0f);
    fireParam.isInfinite = true;
    particleEmitter3->SetParam(fireParam);
    ParticleManager::getInstance()->SetGroupScrollSpeed("Cylinder", { 0.2f, 0.0f });
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{

    Input* input = GetInput();
    Camera* camera = GetCamera();

    skydox->SetCamera(camera);

    if (input->TriggerKey(DIK_F1)) {
        SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
    }

    skydox->Update();

    object3d->Update();

    particleEmitter->Update();
    particleEmitter2->Update();
    particleEmitter3->Update();

    // IMGUI
    object3d->DrawImGui();
}

void TitleScene::Draw()
{

    //
    // モデルデータ
    //
    Object3dCommon::GetInstance()->PrepareObjectDraw();

    object3d->Draw();

    SkyBoxCommon::GetInstance()->PrepareObjectDraw();
    skydox->Draw();

    SpriteCommon::GetInstance()->PrepareSpriteDraw();

    ParticleManager::getInstance()->Draw();
}
