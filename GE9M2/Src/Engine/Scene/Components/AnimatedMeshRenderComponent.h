#pragma once
#include <vector>
#include <string>
#include "../Component.h"
#include "../../Graphics/Animation/AnimationController.h"

class ModelData;
class AnimationData;
class AnimationController;

class AnimatedMeshRenderComponent : public RenderComponent {

private:
    ModelData*             _data;
    AnimationController    _animationController;
    std::string            _currentAnimation        = "run";

public:

    AnimatedMeshRenderComponent(ModelData* data);

    ~AnimatedMeshRenderComponent() = default;

    void playAnimation(const std::string& name);

    void onUpdate(float dt) override;

    void onRender(RenderContext& renderContext) override;

    static std::string Name() { return "AnimatedMeshRenderComponent"; }

};
