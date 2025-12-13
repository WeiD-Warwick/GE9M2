#pragma once
#include <vector>
#include <string>
#include "../Component.h"
#include "../../Graphics/Animation/AnimationController.h"

using namespace std;

class DX12Mesh;
class AnimationData;
class AnimationController;

class AnimatedMeshRenderComponent : public Component {

private:
    vector<DX12Mesh*>      _meshes;
    vector<string>         _textureFilenames;
    AnimationData*         _animation               = nullptr;
    AnimationController    _animationController;
    string                 _shaderName              = "animatedMeshShader";
    string                 _psoName                 = "animatedMeshPSO";
    string                 _constBufferName         = "animatedMeshBuffer";
    string                 _currentAnimation        = "run";

public:

    AnimatedMeshRenderComponent(vector<DX12Mesh*>& meshes, vector<string>& textureFilenames, AnimationData* animationPtr);

    ~AnimatedMeshRenderComponent();

    void playAnimation(const std::string& name);

    void onUpdate(float dt) override;

    void onRender(RenderContext& renderContext) override;

};
