#include "Scene.h"
#include "GameObject.h"
#include "../Engine.h"
#include "../ModelLoader.h"
#include "Component.h"
#include "Components/ColliderComponent.h"
#include "Components/CameraComponent.h"

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

void Scene::destroyObject(GameObject* obj) {
    _objects.erase(
        std::remove(_objects.begin(), _objects.end(), obj),
        _objects.end()
    );
    delete obj;
}

void Scene::update(float dt) {
    for (auto& obj : _objects)
        obj->update(dt);
}

void Scene::render(RenderContext& renderContext) {
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

