#pragma once
#include <vector>

class Component;
class Scene;
class Engine;
class Transform;
class RenderContext;
class CameraComponent;

class GameObject {
protected:
    std::vector<Component*> _components;
    Transform* _transform = nullptr;

    Scene* _scene = nullptr;
    Engine* _engine = nullptr;

public:

    GameObject();

    ~GameObject();

    void setContext(Scene* scene, Engine* engine);

    Transform& transform();
    Scene* scene() const;
    Engine* engine() const;
    CameraComponent* mainCamera();

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        T* component = new T(std::forward<Args>(args)...);
        component->setOwner(this);
        _components.push_back(component);

        component->onStart();
        return component;
    }

    template<typename T>
    T* getComponent() {
        for (Component* component : _components) {
            if (auto result = dynamic_cast<T*>(component))
                return result;
        }
        return nullptr;
    }

    void update(float dt);

    const std::vector<Component*>& components() const;
};
