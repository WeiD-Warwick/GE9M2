#include "FPSRenderComponent.h"
#include "CameraComponent.h"
#include "../../Engine.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Graphics/Material/Material.h"
#include "../../Graphics/Model/ModelData.h"
#include "../../Graphics/Material/Material.h"

FPSRenderComponent::FPSRenderComponent(ModelData* data) : _data(data) {
    _animationController.init(_data->animation);

    _animationController.play("04 idle");
    _state = WeaponAnimState::Idle;

}

void FPSRenderComponent::playAnimation(const std::string& name) {
    if (_currentAnimation != name) {
        _currentAnimation = name;
    }
}

void FPSRenderComponent::onStart() {
    auto& ctx = engine()->renderContext();
    auto& matMgr = ctx.materialManager();

    for (auto& subMesh : _data->subMeshes) {
        _materials.push_back(
            matMgr.createInstance(ctx, subMesh)
        );
    }
}

void FPSRenderComponent::onUpdate(float dt) {

    switch (_state) {
    case WeaponAnimState::Idle:
        switch (_intent) {
        case WeaponIntent::Fire:
            _animationController.play("08 fire");
            _state = WeaponAnimState::Fire;
            break;

        case WeaponIntent::Reload:
            _animationController.play("17 reload");
            _state = WeaponAnimState::Reload;
            break;

        default:
            break;
        }

        _intent = WeaponIntent::None;
        break;

    case WeaponAnimState::Fire:
        if (_animationController.animationFinished()) {
            _animationController.play("04 idle");
            _state = WeaponAnimState::Idle;
        }
        break;

    case WeaponAnimState::Reload:
        if (_animationController.animationFinished()) {
            _animationController.play("04 idle");
            _state = WeaponAnimState::Idle;
        }
        break;
    }

    _animationController.update(dt);
}


void FPSRenderComponent::onRender(RenderContext& renderContext) {

    ID3D12GraphicsCommandList4* commandList = renderContext.renderer().commandList();

    MaterialParam param;
    // Rotate model to right direction
    Matrix armsRotateFix = Matrix::RotateY(M_PI);
    Matrix armsOffset = Matrix::Translation(_baseOffset);

    // Move model to right position
    param.W = armsRotateFix * armsOffset;
    // The weapon will fix on the screen, so it won't use camera view matrix
    param.V = Matrix::Identity();
    // use projection matrix to ensure the right perspective
    param.P = mainCamera()->projection;
    param.bones = _animationController.skinningMatrices;

    for (int i = 0; i < _data->subMeshes.size(); ++i) {
        _materials[i]->apply(renderContext, param);
        _data->subMeshes[i].mesh->draw(commandList);
    }
}

void FPSRenderComponent::setIntent(WeaponIntent intent) {
    if (_state != WeaponAnimState::Idle) {
        return;
    }

    if (_intent == WeaponIntent::None) {
        _intent = intent;
    }
}
