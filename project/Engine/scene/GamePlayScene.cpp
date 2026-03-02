#include "GamePlayScene.h"

#include "../base/WinApp.h"

#include "../2d/SpriteCommon.h"
#include "../base/TextureManager.h"

#include "../3d/Model.h"
#include "../3d/ModelManager.h"
#include "../3d/Object3d.h"
#include "../3d/Object3dCommon.h"

#include "../3d/ParticleEmitter.h"
#include "../3d/ParticleManager.h"

#include "../io/Input.h"
#include "SceneManager.h"
#include "TitleScene.h"

#include "math.h"

void GamePlayScene::Finalize()
{
    fanfare.Unload();
    clearSe.Unload();
}

void GamePlayScene::Initialize()
{

    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    TextureManager::getInstance()->LoadTexture("resources/monsterBall.png");

    for (uint32_t i = 0; i < 1; ++i) {
        auto sprite = std::make_unique<Sprite>();

        if (i % 2 == 0) {
            sprite->Initialize("resources/uvChecker.png");
            sprite->SetPosition(Vector2(100.0f, 100.0f));
        } else {
            sprite->Initialize("resources/monsterBall.png");
        }

        sprites.push_back(std::move(sprite));
    }

    ModelManager::GetInstance()->LoadModel("Plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");

    object3d = std::make_unique<Object3d>();
    object3d->Initialize();

    model = std::make_unique<Model>();
    model->Initialize("resources", "plane.obj");
    object3d->SetModel(model.get());

    object3d2 = std::make_unique<Object3d>();
    object3d2->Initialize();

    model2 = std::make_unique<Model>();
    model2->Initialize("resources", "axis.obj");
    object3d2->SetModel(model2.get());

    ParticleManager::getInstance()->CreateParticleGroup("pori", "resources/circle.png");

    // 板ポリ
    Transform emitter {};
    emitter.translate = { 0.0f, 0.0f, 0.0f };
    emitter.rotate = { 0.0f, 0.0f, 0.0f };
    emitter.scale = { 1.0f, 1.0f, 1.0f };
    particleEmitter = std::make_unique<ParticleEmitter>("pori", emitter, 1.0f, 3);

    fanfare.SoundLoadFile("resources/fanfare.wav");
    clearSe.SoundLoadFile("resources/stage.mp3");

    // 音がうるさいので停止中
    // Audio::GetInstance()->Play(fanfare);
    // Audio::GetInstance()->Play(clearSe);
}

void GamePlayScene::Update()
{
    Input* input = BaseScene::GetInput();

    if (input->TriggerKey(DIK_0)) {
        auto scene = std::make_unique<TitleScene>();
        scene->SetInput(GetInput());

        SceneManager::GetInstance()->SetNextScene(std::move(scene));
    }

    for (auto& sprite : sprites) {
        sprite->Update();
    }

    object3d->Update();

    object3d2->Update();
    Vector3 rotate2 = object3d2->GetRotate();

    object3d2->SetRotate(rotate2);

    particleEmitter->Update();
}

void GamePlayScene::Draw()
{

    Object3dCommon::GetInstance()->PrepareObjectDraw();

    //
    // モデルデータ
    //
    object3d->Draw();
    object3d2->Draw();

    //
    // 2d/スプライト
    //
    SpriteCommon::GetInstance()->PrepareSpriteDraw();

    for (auto& sprite : sprites) {
        sprite->Draw();
    }

    ParticleManager::getInstance()->Draw();
}