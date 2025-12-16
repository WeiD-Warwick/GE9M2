#include "ComponentFactory.h"
#include "GameObject.h"
#include "Component.h"
#include "Components/CameraComponent.h"
#include "Components/PlayerControllerComponent.h"
#include "Components/StaticMeshRenderComponent.h"
#include "Components/AnimatedMeshRenderComponent.h"
#include "Components/FPSRenderComponent.h"
#include "Components/SkySphereRenderComponent.h"
#include "Components/ColliderComponent.h"
#include "../Graphics/Assets/ModelData.h"
#include <cassert>

static ModelLoadArgs loadFromComponentArgs(const ComponentArgs& args) {
    assert(args.size() >= 4);
    ModelLoadArgs loadArgs;
    loadArgs.filePath = args[0];
    loadArgs.shaderName = args[1];
    loadArgs.cbufferName = args[2];
    loadArgs.psoName = args[3];

    if (args.size() > 4) {
        loadArgs.textureName = args[4];
    }

    if (args.size() > 6) {
        loadArgs.uvScale = { std::stof(args[5]), std::stof(args[6]) };
    }
    return loadArgs;
}

ComponentFactory& ComponentFactory::shared() {
    static ComponentFactory shared;

    static bool registered = false;
    if (!registered) {
        shared.registerAllComponents();
        registered = true;
    }

    return shared;
}

void ComponentFactory::registerComponent(const std::string& name, ComponentCreator creator) {
    _creators[name] = creator;
}

bool ComponentFactory::create(const std::string& name, GameObject* obj, Engine& engine, const ComponentArgs& args) {
    auto it = _creators.find(name);
    if (it == _creators.end()) {
        return false;
    }

    it->second(obj, engine, args);
    return true;
}

void ComponentFactory::registerAllComponents() {

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
            ModelLoadArgs loadArgs = loadFromComponentArgs(args);
            auto* model = engine.loader().loadModel(loadArgs);
            obj->addComponent<StaticMeshRenderComponent>(model);
        }
    );

    registerComponent(
        AnimatedMeshRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            ModelLoadArgs loadArgs = loadFromComponentArgs(args);
            auto* model = engine.loader().loadModel(loadArgs);
            obj->addComponent<AnimatedMeshRenderComponent>(model);
        }
    );

    registerComponent(
        FPSRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            ModelLoadArgs loadArgs = loadFromComponentArgs(args);
            auto* model = engine.loader().loadModel(loadArgs);
            obj->addComponent<FPSRenderComponent>(model);
        }
    );

    registerComponent(
        SkySphereRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            ModelLoadArgs loadArgs = loadFromComponentArgs(args);
            auto* model = engine.loader().loadModel(loadArgs);
            obj->addComponent<SkySphereRenderComponent>(model);
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