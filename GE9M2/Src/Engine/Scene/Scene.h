#pragma once
#include <vector>
#include "../Graphics/Material/Material.h"
#include "Component.h"
#include "Light.h"

class GameObject;
class Engine;
class CameraComponent;
class RenderContext;

class Scene {
private:
    std::vector<GameObject*> _objects;
    std::vector<PointLight> _lights;
    SkyLight _skyLight;

    Engine* _engine = nullptr;
	CameraComponent* _mainCamera = nullptr;

public:

    Scene(Engine* engine);

    GameObject* createObject();

    void addLight(const PointLight& light);

    void setSkyLight(const SkyLight& light);

    void uploadLights(RenderContext& ctx);

    void update(float dt);

    void render(RenderContext& renderContext);

    void renderLayer(RenderContext& ctx, RenderLayer layer);

    void setMainCamera(CameraComponent* cam);

    const std::vector<GameObject*>& objects() const;

    CameraComponent* mainCamera();

    ~Scene();

};
