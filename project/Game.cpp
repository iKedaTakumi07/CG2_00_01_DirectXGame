#include "Game.h"

#include "D3dResourceLeakChecker.h"

#include "ImGuiManager.h"
#include "Input.h"
#include "Logger.h"
#include "Math.h"
#include "Object3dCommon.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "SrvManager.h"

#include "Audio.h"
#include "StringUtility.h"
#include "TextureManager.h"

#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "Object3d.h"
#include "Sprite.h"
#include "SpriteCommon.h"

void Game::Initialize()
{

    // 基底クラスの初期化処理
    Framework::Initialize();

    TextureManager::getInstance()->LoadTexture("resources/uvChecker.png");
    TextureManager::getInstance()->LoadTexture("resources/monsterBall.png");

    ModelManager::GetInstance()->LoadModel("Plane.obj");
    ModelManager::GetInstance()->LoadModel("axis.obj");

    ParticleManager::getInstance()->CreateParticleGroup("pori", "resources/circle.png");

    fanfare.SoundLoadFile("resources/fanfare.wav");

    clearSe.SoundLoadFile("resources/stage.mp3");

    audio.Play(fanfare);
    audio.Play(clearSe);

    for (uint32_t i = 0; i < 1; ++i) {
        Sprite* sprite = new Sprite();
        if (i % 2 == 0) {
            sprite->Initialize(spriteCommon, winApp, "resources/uvChecker.png");
            sprite->SetPosition(Vector2(100.0f, 100.0f));
        } else {
            sprite->Initialize(spriteCommon, winApp, "resources/monsterBall.png");
        }

        sprites.push_back(sprite);
    }

    object3d2 = new Object3d();
    object3d2->Initialize(object3dCommon, winApp);

    model2 = new Model();
    model2->Initialize(modelCommon, "resources", "plane.obj");
    object3d2->SetModel("axis.obj");

    object3d = new Object3d();
    object3d->Initialize(object3dCommon, winApp);

    model = new Model();
    model->Initialize(modelCommon, "resources", "plane.obj");
    object3d->SetModel(model);

    // 板ポリ
    Transform emitter {};
    emitter.translate = { 0.0f, 0.0f, 0.0f };
    emitter.rotate = { 0.0f, 0.0f, 0.0f };
    emitter.scale = { 1.0f, 1.0f, 1.0f };
    particleEmitter = new ParticleEmitter("pori", emitter, 1.0f, uint32_t(3));
}

void Game::Update()
{
    // update/更新処理
    Framework::Update();

    if (input->TriggerKey(DIK_0)) {
        OutputDebugStringA("hit 0\n");
    }

    if (input->PushKey(DIK_1)) {
        OutputDebugStringA("hit 1\n");
    }

    for (uint32_t i = 0; i < sprites.size(); ++i) {
        sprites[i]->Update();
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

void Game::Draw()
{
    // draw

    dxCommon->PreDraw();

    srvManager->PreDraw();

    object3dCommon->PrepareObjectDraw();

    //
    // 2d/スプライト
    //
    spriteCommon->PrepareSpriteDraw();

    for (uint32_t i = 0; i < sprites.size(); ++i) {
        sprites[i]->Draw();
    }

    // ParticleManager::getInstance()->Draw();

    // object3d->Draw();
    // object3d2->Draw();

    //
    // モデルデータ
    //

    // 実際のcommandListのImGuiの描画コマンドを詰む
    imguiManager->Draw();

    dxCommon->PostDraw();
}

void Game::Finalize()
{
    Framework::Finalize();

    fanfare.Unload();
    clearSe.Unload();

    for (uint32_t i = 0; i < sprites.size(); ++i) {
        delete sprites[i];
    }
    delete object3d;
    delete object3d2;
    delete model;
    delete model2;
}