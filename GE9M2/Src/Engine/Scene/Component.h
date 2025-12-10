#pragma once
#include <string>
#include "../Graphics/RenderContext.h"

class GameObject;
class Engine;
class Scene;

// =============================================================
//          Necessary Components Order 
// 
// PlayerControllerComponent    - 10
// MouseLookComponent           - 20
// MovementComponent            - 30
// CameraComponent              - 40
// StaticMeshRenderComponent    - 100
// =============================================================

class Component {
public:
    GameObject* owner = nullptr;   // the owner of this component
    Scene* scene = nullptr;
    Engine* engine = nullptr;

public:
    virtual ~Component() {}

    virtual int getUpdateOrder() const { return 100; }

    // Called after component is attached to GameObject
    virtual void onStart() {}

    // Per-frame update logic, before render
    virtual void onUpdate(float dt) {}

    // Per-frame rendering callback for mesh
    virtual void onRender(RenderContext& renderContext) {}

    // For debugging
    virtual const std::string& getName() const { 
        static std::string name = "Component";
        return name;
    }
};
