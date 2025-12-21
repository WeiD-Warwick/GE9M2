#include "ComponentFactory.h"
#include <cassert>
#include "../../GameObject.h"
#include "../../../Graphics/Model/ModelData.h"
#include "../Component.h"
#include "../CameraComponent.h"
#include "../ColliderComponent.h"
#include "../Animator/AnimatorComponent.h"
#include "../Controller/PlayerInputControllerComponent.h"
#include "../Controller/WeaponControllerComponent.h"
#include "../Controller/CowControllerComponent.h"
#include "../Render/AnimatedMeshRenderComponent.h"
#include "../Render/StaticMeshRenderComponent.h"
#include "../Render/SkyRenderComponent.h"
#include "../Render/FPSWeaponRenderComponent.h"
#include "../Render/FogParticleComponent.h"

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

    // =====================================================
    // Default Component
    // =====================================================
    registerComponent(
        CameraComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs&) {
            float aspect = engine.renderContext().aspectRatio();
            obj->addComponent<CameraComponent>(aspect);
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

    // =====================================================
    // Controller Component
    // =====================================================

    registerComponent(
        PlayerInputControllerComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs&) {
            obj->addComponent<PlayerInputControllerComponent>();
        }
    );

    registerComponent(
        WeaponControllerComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            assert(args.size() >= 3);
            WeaponAnimConfig config;
            config.idle = args[0];
            config.fire = args[1];
            config.reload = args[2];
            obj->addComponent<WeaponControllerComponent>(config);
        }
    );

    registerComponent(
        CowControllerComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            assert(args.size() >= 3);
            CowAnimConfig config;
            config.idle = args[0];
            config.hit = args[1];
            config.death = args[2];
            obj->addComponent<CowControllerComponent>(config);
        }
    );

    // =====================================================
    // Mesh Render Component
    // =====================================================

    registerComponent(
        AnimatedMeshRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
            

            if (model->animation) {
                obj->addComponent<AnimatorComponent>(model->animation);
            }
            obj->addComponent<AnimatedMeshRenderComponent>(model->subMeshes);
        }
    );

    registerComponent(
        FPSWeaponRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
            

            if (model->animation) {
                obj->addComponent<AnimatorComponent>(model->animation);
            }
            obj->addComponent<FPSWeaponRenderComponent>(model->subMeshes);
        }
    );

    // =====================================================
    // Static Mesh Render Component
    // =====================================================

    registerComponent(
        SkyRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
            obj->addComponent<SkyRenderComponent>(model->subMeshes);
        }
    );

    registerComponent(
        StaticMeshRenderComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            auto* model = engine.loader().loadModel(args[0], args[1]);
            obj->addComponent<StaticMeshRenderComponent>(model->subMeshes);
        }
    );

    registerComponent(
        FogParticleComponent::Name(),
        [](GameObject* obj, Engine& engine, const ComponentArgs& args) {
            assert(args.size() >= 10);
            const std::string& matKey = args[0];
            int count = std::stoi(args[1]);
            float radius = std::stof(args[2]);
            float minH = std::stof(args[3]);
            float maxH = std::stof(args[4]);
            float minS = std::stof(args[5]);
            float maxS = std::stof(args[6]);
            float density = std::stof(args[7]);
            float opacity = std::stof(args[8]);
            float noiseScale = std::stof(args[9]);

            obj->addComponent<FogParticleComponent>(
                matKey,
                count,
                radius,
                minH,
                maxH,
                minS,
                maxS,
                density,
                opacity,
                noiseScale
            );
        }
    );
}