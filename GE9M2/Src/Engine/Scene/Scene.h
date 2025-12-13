#pragma once
#include <vector>

class GameObject;
class Engine;
class CameraComponent;
class RenderContext;

class Scene {
private:
    std::vector<GameObject*> objects;
    Engine* _engine = nullptr;
	CameraComponent* _mainCamera = nullptr;

public:

    Scene(Engine* engine);

    GameObject* createObject();

    void destroyObject(GameObject* obj);

    void update(float dt);

    void render(RenderContext& renderContext);

    void setMainCamera(CameraComponent* cam);

    CameraComponent* mainCamera();

    ~Scene();

};
