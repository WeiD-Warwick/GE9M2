#pragma once
#include <vector>
#include <string>
#include "../Component.h"
#include "../../Graphics/Animation/AnimationController.h"

class ModelData;
class AnimationData;
class AnimationController;
class Material;

class AnimatedMeshRenderComponent : public RenderComponent {

private:
    ModelData*             _data;
    Material*              _material;
    AnimationController    _animationController;
    std::string            _psoName                 = "animatedMeshPSO";
    std::string            _currentAnimation        = "run";

public:

    AnimatedMeshRenderComponent(ModelData* data, Material* material);

    ~AnimatedMeshRenderComponent();

    void playAnimation(const std::string& name);

    void onUpdate(float dt) override;

    void onRender(RenderContext& renderContext) override;

};
