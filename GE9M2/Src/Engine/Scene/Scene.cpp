#include "Scene.h"
#include <algorithm>
#include "GameObject.h"
#include "Component.h"
#include "Components/ColliderComponent.h"
#include "Components/CameraComponent.h"
#include "../Engine.h"
#include "../Graphics/Assets/ModelLoader.h"


Scene::Scene(Engine* engine) : _engine(engine) {}

Scene::~Scene() {
    for (auto* obj : _objects)
        delete obj;
}

GameObject* Scene::createObject() {
    GameObject* obj = new GameObject();
    obj->setContext(this, _engine);
    _objects.push_back(obj);
    return obj;
}

void Scene::addLight(const PointLight& light) {
    _lights.push_back(light);
}

void Scene::setSkyLight(const SkyLight& light) {
    _skyLight = light;
}

void Scene::uploadLights(RenderContext& ctx) {
    const int MAX_POINT_LIGHTS = 10;
    auto& shaderManager = ctx.shaderManager();
    // Sky Light
    Vec3 skyColor = _skyLight.color;
    float skyIntensity = _skyLight.intensity;

    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "skyLightColor", &skyColor);
    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "skyLightIntensity", &skyIntensity);

    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "skyLightColor", &skyColor);
    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "skyLightIntensity", &skyIntensity);

    // Point Light

    Vec3  lightPosWS[MAX_POINT_LIGHTS] = {};
    float lightRange[MAX_POINT_LIGHTS] = {};
    Vec3  lightColor[MAX_POINT_LIGHTS] = {};
    float lightIntensity[MAX_POINT_LIGHTS] = {};

    int count = _lights.size() > MAX_POINT_LIGHTS ? MAX_POINT_LIGHTS : _lights.size();

    for (int i = 0; i < count; ++i) {
        const PointLight& light = _lights[i];

        lightPosWS[i] = light.position;
        lightRange[i] = light.range;
        lightColor[i] = light.color;
        lightIntensity[i] = light.intensity;
    }

    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "pointLightCount", &count);
    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "lightPosWS", lightPosWS);
    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "lightRange", lightRange);
    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "lightColor", lightColor);
    shaderManager.updateConstantPS("staticMeshShader", "staticMeshBuffer", "lightIntensity", lightIntensity);

    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "pointLightCount", &count);
    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "lightPosWS", lightPosWS);
    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "lightRange", lightRange);
    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "lightColor", lightColor);
    shaderManager.updateConstantPS("animatedMeshShader", "animatedMeshBuffer", "lightIntensity", lightIntensity);
}

void Scene::update(float dt) {
    for (auto& obj : _objects)
        obj->update(dt);
}

void Scene::render(RenderContext& renderContext) {

    uploadLights(renderContext);

    renderLayer(renderContext, RenderLayer::Sky);
    renderLayer(renderContext, RenderLayer::World);
    renderLayer(renderContext, RenderLayer::FPS);

    if (_engine->showCollisionBoxes) {
        renderLayer(renderContext, RenderLayer::DEBUG);
    }
}

void Scene::renderLayer(RenderContext& renderContext, RenderLayer layer) {
    for (auto* obj : _objects) {
        for (auto* comp : obj->components()) {
            if (auto* render = dynamic_cast<Renderable*>(comp)) {
                if (render->layer() == layer)
                    render->onRender(renderContext);
            }
        }
    }
}

void Scene::setMainCamera(CameraComponent* cam) {
    _mainCamera = cam;
}

const std::vector<GameObject*>& Scene::objects() const {
    return _objects;
}

CameraComponent* Scene::mainCamera() {
    return _mainCamera;
}