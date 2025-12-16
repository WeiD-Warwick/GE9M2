#include "PlayerControllerComponent.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../Foundation/Window/Window.h"
#include "FPSRenderComponent.h"
#include "../GameObject.h"
#include "../Scene.h"
#include "../../Engine.h"
#include "ColliderComponent.h"

void PlayerControllerComponent::onStart() {
    Vec3 f = transform().forward();
    if (f.lengthSqrt() > 0.0001f) {
        f = f.normalized();
        _yaw = std::atan2(f.x, f.z);
        _pitch = std::asin(_clamp(f.y, -1.0f, 1.0f));
    }
    else {
        _yaw = 0.0f;
        _pitch = 0.0f;
    }
}

void PlayerControllerComponent::onUpdate(float dt) {
    if (!window || !_owner) return;

    // ------------------- ESC -------------------
    //bool esc = window->keys[VK_ESCAPE];
    //if (esc && !_lastEsc) {
    //    engine()->continueLoop = false;
    //    return;
    //}
    //_lastEsc = esc;


    // ------------------- Mouse look -------------------
    float dx = window->mouseDeltaX;
    float dy = window->mouseDeltaY;
    window->mouseDeltaX = 0;
    window->mouseDeltaY = 0;

    _yaw += dx * _mouseSensitivity;
    _pitch += dy * _mouseSensitivity;

    _pitch = _clamp(_pitch, -1.5f, 1.5f);

    // calculate Yaw rotation and apply to transform
    Quaternion qYaw = Quaternion::fromAxisAngle(Vec3(0, 1, 0), _yaw);
    transform().rotation = qYaw;

    // calculate pitch rotation BUT NOT APPLY(SAVE AND USE FOR CAMERA)
    Vec3 localRight = qYaw.rotate(Vec3(1, 0, 0));
    Quaternion qPitch = Quaternion::fromAxisAngle(localRight, _pitch);

    //transform().rotation = (qPitch * qYaw).normalized();
    _fullLookRotation = (qPitch * qYaw).normalized();

    // ------------------- Mouse look -------------------
    float inputX = 0.0f;
    float inputZ = 0.0f;

    if (window->keys['W']) inputZ += 1.0f;
    if (window->keys['S']) inputZ -= 1.0f;
    if (window->keys['A']) inputX -= 1.0f;
    if (window->keys['D']) inputX += 1.0f;

    Vec3 forward = transform().forward();
    forward.y = 0.0f;

    Vec3 right = transform().right();
    right.y = 0.0f;
    Vec3 moveDir = (forward * inputZ + right * inputX).normalized();
    // cache oldPose
    Vec3 oldPos = transform().position;
    Vec3 newPos = oldPos + moveDir * (_moveSpeed * dt);
    // apply movement
    transform().position += moveDir * (_moveSpeed * dt);

    // ------------------- Collision Checker -------------------
    auto* selfCollider = _owner->getComponent<ColliderComponent>();

    // --- X axis ---
    transform().position.x = newPos.x;
    for (auto* obj : scene()->objects()) {
        if (obj == _owner) continue;
        auto* other = obj->getComponent<ColliderComponent>();
        if (!other) continue;

        if (selfCollider->intersect(other)) {
            transform().position.x = oldPos.x;
            break;
        }
    }

    // --- Z axis ---
    transform().position.z = newPos.z;
    for (auto* obj : scene()->objects()) {
        if (obj == _owner) continue;
        auto* other = obj->getComponent<ColliderComponent>();
        if (!other) continue;

        if (selfCollider->intersect(other)) {
            transform().position.z = oldPos.z;
            break;
        }
    }

    // ------------------- Player Animation -------------------
    // Fire
    bool mouseLeft = window->mouseButtons[0];
    if (mouseLeft && !_lastMouseLeft) {
        if (FPSRenderComponent* arms = _owner->getComponent<FPSRenderComponent>()) {
            arms->setIntent(WeaponIntent::Fire);
        }
    }

    _lastMouseLeft = mouseLeft;


    // Reload
    bool reloadKey = window->keys['R'];

    if (reloadKey && !_lastReloadKey) {
        if (FPSRenderComponent* arms = _owner->getComponent<FPSRenderComponent>()) {
            arms->setIntent(WeaponIntent::Reload);
        }
    }

    _lastReloadKey = reloadKey;
}