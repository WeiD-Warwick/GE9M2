#include "FPSGame.h"
#include "../Engine/Engine.h"
#include "../Engine/Scene/Scene.h"
#include "../Engine/Scene/GameObject.h"
#include "../Engine/Scene/Components/CameraComponent.h"
#include "../Engine/Scene/Components/PlayerControllerComponent.h"
#include "../Engine/Scene/Components/FPSRenderComponent.h"
#include "Level/LevelLoader.h"
#include "../Engine/Graphics/Material/ModelMaterial.h"

void FPSGame::onInit(Engine& engine, Scene& scene) {

    _levelLoader.loadLevel(engine, scene, "Src/Assets/Levels/level_demo.txt");

}
