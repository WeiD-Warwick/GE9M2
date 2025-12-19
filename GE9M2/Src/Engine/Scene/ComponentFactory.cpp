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
#include "../Graphics/Model/ModelData.h"
#include <cassert>

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
            auto* model = engine.loader().loadModel(args[0], args[1]);
            obj->addComponent<StaticMeshRenderComponent>(model);
        }
    );

    registerComponent(
        AnimatedMeshRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
            obj->addComponent<AnimatedMeshRenderComponent>(model);
        }
    );

    registerComponent(
        FPSRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
            obj->addComponent<FPSRenderComponent>(model);
        }
    );

    registerComponent(
        SkySphereRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
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