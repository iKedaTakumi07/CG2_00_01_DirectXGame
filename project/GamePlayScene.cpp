#include "GamePlayScene.h"

#include "WinApp.h"

// #include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"

#include "Model.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"

#include "ParticleEmitter.h"
#include "ParticleManager.h"

#include "Input.h"
#include "SceneManager.h"
#include "TitleScene.h"

void GamePlayScene::Finalize()
{
    fanfare.Unload();
    clearSe.Unload();

    delete particleEmitter;
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
    object3d->Initialize(/*object3dCommon, winapp*/);

    model = std::make_unique<Model>();
    model->Initialize(/*modelCommon,*/ "resources", "plane.obj");
    object3d->SetModel(model.get());

    object3d2 = std::make_unique<Object3d>();
    object3d2->Initialize(/*object3dCommon, winApp*/);

    model2 = std::make_unique<Model>();
    model2->Initialize(/*modelCommon,*/ "resources", "axis.obj");
    object3d2->SetModel(model2.get());

    ParticleManager::getInstance()->CreateParticleGroup("pori", "resources/circle.png");

    // 板ポリ
    Transform emitter {};
    emitter.translate = { 0.0f, 0.0f, 0.0f };
    emitter.rotate = { 0.0f, 0.0f, 0.0f };
    emitter.scale = { 1.0f, 1.0f, 1.0f };
    particleEmitter = new ParticleEmitter("pori", emitter, 1.0f, uint32_t(3));

    fanfare.SoundLoadFile("resources/fanfare.wav");
    clearSe.SoundLoadFile("resources/stage.mp3");

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

    Vector3 rotate = object3d->GetRotate();
    rotate.x += 0.1f;
    rotate.y += 0.1f;
    object3d->SetRotate(rotate);
    object3d->Update();

    object3d2->Update();
    Vector3 rotate2 = object3d2->GetRotate();
    rotate2.x += -0.1f;
    rotate2.y += -0.1f;
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