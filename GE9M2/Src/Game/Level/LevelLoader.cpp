#include "LevelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include "../../Engine/Engine.h"
#include "../../Engine/Scene/Scene.h"
#include "../../Engine/Scene/GameObject.h"
#include "../../Engine/Graphics/Material/ModelMaterial.h"
// Components
#include "../../Engine/Scene/Components/StaticMeshRenderComponent.h"
#include "../../Engine/Scene/Components/CameraComponent.h"
#include "../../Engine/Scene/Components/PlayerControllerComponent.h"
#include "../../Engine/Scene/Components/AnimatedMeshRenderComponent.h"
#include "../../Engine/Scene/Components/SkySphereRenderComponent.h"
#include "../../Engine/Scene/Components/FPSRenderComponent.h"
#include "../../Engine/Scene/Components/ColliderComponent.h"


using ComponentArgs = std::vector<std::string>;
using ComponentCreator = std::function<void(GameObject*, Engine&, const ComponentArgs&)>;

static std::unordered_map<std::string, ComponentCreator> ComponentContainer;

static void registerComponent(const std::string& name, ComponentCreator creator) {
    ComponentContainer[name] = creator;
}

// args[0] = model path
// args[1] = shader name
// args[2] = cbuffer name
static void registerAllComponents() {

    registerComponent(
        CameraComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs&) {
            float aspect = engine.renderContext().aspectRatio();
            obj->addComponent<CameraComponent>(aspect);
        }
    );

    registerComponent(
        PlayerControllerComponent::Name(),
        [](GameObject* obj, Engine&, const ComponentArgs&) {
            obj->addComponent<PlayerControllerComponent>();
        }
    );

    registerComponent(
        StaticMeshRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModelFromFile(args[0]);
            auto* material = new ModelMaterial(args[1], args[2]);
            obj->addComponent<StaticMeshRenderComponent>(model, material);
        }
    );

    registerComponent(
        AnimatedMeshRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModelFromFile(args[0]);
            auto* material = new ModelMaterial(args[1], args[2]);
            obj->addComponent<AnimatedMeshRenderComponent>(model, material);
        }
    );

    registerComponent(
        FPSRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModelFromFile(args[0]);
            auto* material = new ModelMaterial(args[1], args[2]);
            obj->addComponent<FPSRenderComponent>(model, material);
        }
    );

    registerComponent(
        SkySphereRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModelFromFile(args[0]);
            auto* material = new ModelMaterial(args[1], args[2]);
            obj->addComponent<SkySphereRenderComponent>(model, material);
        }
    );

    registerComponent(
        ColliderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            assert(args.size() >= 3);
            Vec3 size(
                std::stof(args[0]),
                std::stof(args[1]),
                std::stof(args[2])
            );

            obj->addComponent<ColliderComponent>(size);
        }
    );
}

static bool parseTransform(
    std::istream& in,
    Vec3& position,
    Quaternion& rotation,
    Vec3& scale
) { 
    return (in >> position.x >> position.y >> position.z) 
        && (in >> rotation.x >> rotation.y >> rotation.z >> rotation.w) 
        && (in >> scale.x >> scale.y >> scale.z); 
}

void LevelLoader::loadLevel(Engine& engine, Scene& scene, const std::string& levelPath) {
    static bool registered = false;
    if (!registered) {
        registerAllComponents();
        registered = true;
    }

    std::ifstream file(levelPath);

    std::string animatedMeshShaderCBName = "animatedMeshBuffer";
    std::string staticMeshShaderCBName = "staticMeshBuffer";
    std::string skySphereShaderCBName = "skySphereBuffer";

    std::string line;
    GameObject* currentObject = nullptr;

    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty())
            continue;

        // Skip comments
        if (line.rfind("//", 0) == 0)
            continue;

        // ------------------------------------------------------------
        // New GameObject
        // ------------------------------------------------------------
        if (line[0] == '#') {
            currentObject = scene.createObject();
            currentObject->setName(line.substr(1));

            if (!std::getline(file, line)) { break; }

            std::stringstream transformLine(line);

            Vec3 position;
            Quaternion rotation;
            Vec3 scale;

            if (!parseTransform(transformLine, position, rotation, scale)) {
                currentObject = nullptr;
                continue;
            }

            currentObject->transform().position = position;
            currentObject->transform().rotation = rotation;
            currentObject->transform().scale = scale;

            continue;
        }

        // ------------------------------------------------------------
        // Component line (create components belong to this GameObject)
        // ------------------------------------------------------------
        if (!currentObject) { 
            continue;
        }

        std::stringstream componentLine(line);

        std::string componentName;
        componentLine >> componentName;

        if (componentName.empty()) {
            continue;
        }

        // read arguments
        std::vector<std::string> args;
        std::string arg;
        while (componentLine >> arg) {
            args.push_back(arg);
        }

        auto it = ComponentContainer.find(componentName);
        if (it == ComponentContainer.end()) {
            continue;
        }

        // add component to Gameobject
        it->second(currentObject, engine, args);
    }
}