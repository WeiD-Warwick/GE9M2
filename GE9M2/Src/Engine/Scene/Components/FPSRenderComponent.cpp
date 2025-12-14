#include "FPSRenderComponent.h"
#include "CameraComponent.h"
#include "../../Graphics/RenderContext.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../../Graphics/Material/Material.h"
#include "../../ModelLoader.h"

FPSRenderComponent::FPSRenderComponent(ModelData* data, Material* material)
    : _data(data), _material(material) {
    _animationController.init(_data->animation);

    _animationController.play("04 idle");
    _state = WeaponAnimState::Idle;

}

FPSRenderComponent::~FPSRenderComponent() {
    delete _material;
}

void FPSRenderComponent::playAnimation(const std::string& name) {
    if (_currentAnimation != name) {
        _currentAnimation = name;
    }
}

void FPSRenderComponent::onUpdate(float dt) {

    switch (_state) {
    case WeaponAnimState::Idle:
        switch (_intent) {
        case WeaponIntent::Fire:
            _animationController.play("08 fire");
            _state = WeaponAnimState::Fire;

            // recoil impulse
            _recoilVel -= 1.5f;
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

    // Recoil
    _recoilVel += -_recoilZ * 40.0f * dt;
    _recoilVel *= 0.85f;
    _recoilZ += _recoilVel * dt;

    _animationController.update(dt);
}


void FPSRenderComponent::onRender(RenderContext& renderContext) {

    ID3D12GraphicsCommandList4* cmd = renderContext.renderer().commandList();
    PSOManager& psos = renderContext.psoManager();

    psos.bind(cmd, _psoName);

    MaterialParam param;
    // Rotate model to right direction
    Matrix armsRotateFix = Matrix::RotateY(M_PI);
    Matrix armsOffset = Matrix::Translation(_baseOffset + Vec3(0, 0, _recoilZ));

    // Move model to right position
    param.W = armsRotateFix * armsOffset;
    // The weapon will fix on the screen, so it won't use camera view matrix
    param.V = Matrix::Identity();
    // use projection matrix to ensure the right perspective
    param.P = mainCamera()->projection;
    param.bones = _animationController.skinningMatrices;

    for (auto& mesh : _data->meshes) {
        _material->apply(renderContext, mesh->textureNames, param);
        mesh->draw(cmd);
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
