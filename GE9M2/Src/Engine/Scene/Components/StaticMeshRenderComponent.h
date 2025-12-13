#pragma once
#include <vector>
#include <string>
#include "../Component.h"

using namespace std;

class DX12Mesh;
class RenderContext;
class StaticMeshRenderComponent : public Component {

private:
    vector<DX12Mesh*> _meshes;
    vector<string>    _textureFilenames;

    string            _shaderName             = "staticMeshShader";
    string            _psoName                = "staticMeshPSO";
    string            _constBufferName        = "staticMeshBuffer";

public:
    StaticMeshRenderComponent(vector<DX12Mesh*>& meshes, vector<string>& textureFilenames);

    void onRender(RenderContext& renderContext) override;
};
