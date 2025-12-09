#pragma once
#include <vector>
#include "Component.h"
#include "../Foundation/Transform.h"

class GameObject {
public:
    std::vector<Component*> components;

    Scene* scene = nullptr;
    Engine* engine = nullptr;
    Transform transform;

public:

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {

        T* component = new T(std::forward<Args>(args)...);

        component->owner = this;
        component->scene = scene;
        component->engine = engine;

        components.push_back(component);

        component->onStart();
        return component;
    }

    template<typename T>
    T* getComponent() {
        for (Component* component : components) {
            if (auto result = dynamic_cast<T*>(component))
                return result;
        }
        return nullptr;
    }

    void update(float dt) {
        for (Component* component : components)
            component->onUpdate(dt);
    }

    void render(RenderContext& renderContext) {
        for (Component* component : components)
            component->onRender(renderContext);
    }

public:
    GameObject() = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    ~GameObject() {
        for (Component* c : components)
            delete c;
        components.clear();
    }
};
