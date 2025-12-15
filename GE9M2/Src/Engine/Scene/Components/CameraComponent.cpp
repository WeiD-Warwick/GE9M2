#include "CameraComponent.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../Scene.h"

CameraComponent::CameraComponent(float aspectRatio) : _aspectRatio(aspectRatio) {}

void CameraComponent::onStart() {
    if (!scene()->mainCamera()) {
        scene()->setMainCamera(this);
    }
}

void CameraComponent::onUpdate(float dt) {
    updateViewMatrix();
    updateProjectionIfNeeded();
}

void CameraComponent::updateProjectionIfNeeded() {
    if (!_needToUpdateProjetion) return;

    projection = Matrix::perspective(_nearPlane, _farPlane, _aspectRatio, _fov);

    _needToUpdateProjetion = false;
}

void CameraComponent::updateViewMatrix() {

    Vec3 ownerEyePosition = transform().position + Vec3(0.0f, _eyeHeight, 0.0f);
    // -Z is forward
    Vec3 ownerForward = transform().forward();
    Vec3 ownerUp = transform().up();

    view = Matrix::lookAt(ownerEyePosition, ownerEyePosition + ownerForward, ownerUp);
}