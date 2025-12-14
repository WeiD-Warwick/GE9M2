#pragma once
#include <vector>
#include "../Graphics/Material/Material.h"
#include "Component.h"

class GameObject;
class Engine;
class CameraComponent;
class RenderContext;
class Scene {
private:
    std::vector<GameObject*> _objects;
    Engine* _engine = nullptr;
	CameraComponent* _mainCamera = nullptr;

public:

    Scene(Engine* engine);

    GameObject* createObject();

    void destroyObject(GameObject* obj);

    void update(float dt);

    void render(RenderContext& renderContext);

    void renderLayer(RenderContext& ctx, RenderLayer layer);

    void setMainCamera(CameraComponent* cam);

    CameraComponent* mainCamera();

    ~Scene();

};
