#include "FPSGame.h"
#include "../Engine/Engine.h"
#include "../Engine/Scene/Scene.h"
#include "../Engine/Scene/GameObject.h"
#include "../Engine/Scene/Components/CameraComponent.h"
#include "../Engine/Scene/Components/PlayerControllerComponent.h"
#include "../Engine/Graphics/Material/SkySphereMaterial.h"
#include "../Engine/Scene/Components/StaticMeshRenderComponent.h"
#include "../Engine/Scene/Components/SkySphereRenderComponent.h"
#include "../Engine/Graphics/Material/AnimatedMeshMaterial.h"
#include "../Engine/Scene/Components/AnimatedMeshRenderComponent.h"
#include "../Engine/Graphics/Material/StaticMeshMaterial.h"

void FPSGame::onInit(Engine& engine, Scene& scene) {
    createPlayer(engine, scene);
    createGround(engine, scene);
    createDinosaur(engine, scene);
    createSky(engine, scene);
}

void FPSGame::createPlayer(Engine& engine, Scene& scene) {
    GameObject* player = scene.createObject();

    // Camera
    float aspectRatio = engine.renderContext().aspectRatio();
    CameraComponent* camera = player->addComponent<CameraComponent>(aspectRatio);
    scene.setMainCamera(camera);

    // Controller
    player->addComponent<PlayerControllerComponent>();
    player->transform().position = Vec3(0, 5, -5);
}

void FPSGame::createGround(Engine& engine, Scene& scene) {
    GameObject* ground = scene.createObject();
    DX12Mesh* plane = &engine.meshLib().plane;
    plane->textureNames.push_back("Src/Assets/Textures/ground_diffuse.png");
    auto* staticMaterial = new StaticMeshMaterial("staticMeshShader");
    ModelData* modelData = new ModelData();
    modelData->meshes = { plane };

    ground->addComponent<StaticMeshRenderComponent>(modelData, staticMaterial);
    ground->transform().position = { 0, 0, 0 };
    ground->transform().scale = { 100, 1, 100 };
}

void FPSGame::createDinosaur(Engine& engine, Scene& scene) {
    auto* animatedMaterial = new AnimatedMeshMaterial("animatedMeshShader");
    ModelData* modelData = engine.loader().loadModelFromFile("Src/Assets/Models/TRex.gem");
    GameObject* dino = scene.createObject();
    dino->addComponent<AnimatedMeshRenderComponent>(modelData, animatedMaterial);

    dino->transform().position = { 0, 0, 5 };
    dino->transform().scale = { 0.1, 0.1, 0.1 };
}

void FPSGame::createSky(Engine& engine, Scene& scene) {
    GameObject* sky = scene.createObject();
    DX12Mesh* skySphere = &engine.meshLib().skySphere;
    skySphere->textureNames.push_back("Src/Assets/Textures/skySphere.png");

    ModelData* modelData = new ModelData();
    modelData->meshes = { skySphere };

    auto* skyMaterial = new SkySphereMaterial("skySphereShader");

    sky->addComponent<SkySphereRenderComponent>(modelData, skyMaterial);
}