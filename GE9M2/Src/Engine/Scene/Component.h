#pragma once
#include <string>
#include "../Graphics/Pipeline/RenderContext.h"

class GameObject;
class Engine;
class Scene;

class Component {
public:
    GameObject* owner = nullptr;   // the owner of this component
    Scene* scene = nullptr;
    Engine* engine = nullptr;

public:
    virtual ~Component() {}

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
