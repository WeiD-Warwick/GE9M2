#pragma once
#include <vector>
#include <string>
#include "../Component.h"
#include "../../Graphics/Animation/AnimationController.h"

using namespace std;

class ModelData;
class AnimationData;
class AnimationController;
class Material;

class AnimatedMeshRenderComponent : public Component {

private:
    ModelData*             _data;
    Material*              _material;
    AnimationController    _animationController;
    string                 _psoName                 = "animatedMeshPSO";
    string                 _currentAnimation        = "run";

public:

    AnimatedMeshRenderComponent(ModelData* data, Material* material);

    ~AnimatedMeshRenderComponent();

    void playAnimation(const std::string& name);

    void onUpdate(float dt) override;

    void onRender(RenderContext& renderContext) override;

};
