#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "GameObject.h"
#include "Components/CameraComponent.h"

class Scene {
private:
    std::vector<GameObject*> objects;

public:
    Engine* engine = nullptr;
	CameraComponent* mainCamera = nullptr;

public:

    Scene(Engine* engine) : engine(engine) {}

    GameObject* createObject() {
        GameObject* obj = new GameObject();
        obj->scene = this;
        obj->engine = engine;
        objects.push_back(obj);
        return obj;
    }

    void destroyObject(GameObject* obj) {
        objects.erase(
            std::remove(objects.begin(), objects.end(), obj),
            objects.end()
        );
        delete obj;
    }

    void update(float dt) {
        for (auto& obj : objects)
            obj->update(dt);
    }

    void render(RenderContext& renderContext) {
        for (auto& obj : objects)
            obj->render(renderContext);
    }

    void setMainCamera(CameraComponent* cam) {
        mainCamera = cam;
    }

    ~Scene() {
        for (auto* obj : objects)
            delete obj;
    }

};
