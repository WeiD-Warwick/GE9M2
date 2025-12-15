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

    Vec3 ownerPosition = transform().position;
    // -Z is forward
    Vec3 ownerForward = transform().forward();
    Vec3 ownerUp = transform().up();

    view = Matrix::lookAt(ownerPosition, ownerPosition + ownerForward, ownerUp);
}