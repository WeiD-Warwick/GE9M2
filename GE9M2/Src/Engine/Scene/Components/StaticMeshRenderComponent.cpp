#include "StaticMeshRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../../Graphics/Material/Material.h"
#include "../../ModelLoader.h"

using namespace std;

StaticMeshRenderComponent::StaticMeshRenderComponent(ModelData* data, Material* material)
    : _data(data), _material(material) {}

StaticMeshRenderComponent::~StaticMeshRenderComponent() {
    delete _material;
}

void StaticMeshRenderComponent::onRender(RenderContext& renderContext) {
    auto* commandList = renderContext.renderer().commandList();
    auto& psos = renderContext.psoManager();

    psos.bind(commandList, _psoName);

    MaterialParam param;
    param.W = transform().worldMatrix();
    param.V = mainCamera()->view;
    param.P = mainCamera()->projection;
   
   
    for (auto& mesh : _data->meshes) {
        _material->apply(renderContext, mesh->textureNames, param);
        mesh->draw(commandList);
    }
}