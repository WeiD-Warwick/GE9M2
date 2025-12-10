#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "GameObject.h"
#include "Components/CameraComponent.h"

class Scene {
private:
    std::vector<GameObject*> objects;
	float totalTime = 0.0f;

public:
    CameraComponent* mainCamera = nullptr;
    Engine* engine = nullptr;

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
        totalTime += dt;

        float radius = 11.0f;
        float height = 5.0f;
        Vec3 newPos = Vec3(radius * std::cos(totalTime), height, radius * std::sin(totalTime)
        );

        mainCamera->owner->transform.setPosition(newPos);

        Vec3 target = Vec3(0, 0, 0);
        Vec3 up = Vec3(0, 1, 0);
        mainCamera->orientTowards(target, up);


        for (auto& obj : objects)
            obj->update(dt);
    }

    void render(RenderContext& renderContext) {
        if (!mainCamera) return;

        mainCamera->updateViewMatrix();
        mainCamera->updateProjectionIfNeeded();

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
