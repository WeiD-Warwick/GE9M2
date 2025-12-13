#include "Scene.h"
#include "GameObject.h"
#include "../Engine.h"

Scene::Scene(Engine* engine) : _engine(engine) {}

Scene::~Scene() {
    for (auto* obj : objects)
        delete obj;
}

GameObject* Scene::createObject() {
    GameObject* obj = new GameObject();
    obj->setContext(this, _engine);
    objects.push_back(obj);
    return obj;
}

void Scene::destroyObject(GameObject* obj) {
    objects.erase(
        std::remove(objects.begin(), objects.end(), obj),
        objects.end()
    );
    delete obj;
}

void Scene::update(float dt) {
    for (auto& obj : objects)
        obj->update(dt);
}

void Scene::render(RenderContext& renderContext) {
    for (auto& obj : objects)
        obj->render(renderContext);
}

void Scene::setMainCamera(CameraComponent* cam) {
    _mainCamera = cam;
}

CameraComponent* Scene::mainCamera() {
    return _mainCamera;
}

