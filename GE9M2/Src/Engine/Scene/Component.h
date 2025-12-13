#pragma once
#include <string>

class GameObject;
class Scene;
class Engine;
class CameraComponent;
class Transform;
class RenderContext;

class Component {
protected:
    GameObject* _owner = nullptr;

public:

    void setOwner(GameObject* owner);

    Transform& transform();
    Scene* scene();
    Engine* engine();
    CameraComponent* mainCamera();

    // Called after component is attached to GameObject
    virtual void onStart() {}

    // Per-frame update logic, before render
    virtual void onUpdate(float dt) {}

    // Per-frame rendering callback for mesh
    virtual void onRender(RenderContext& renderContext) {}
};
