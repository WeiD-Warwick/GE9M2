#include "PlayerControllerComponent.h"
#include "../../Foundation/Maths.h"
#include "../../Foundation/Transform.h"
#include "../../Platform/Window/Window.h"
#include "FPSRenderComponent.h"
#include "../GameObject.h"
#include "../Scene.h"
#include "ColliderComponent.h"

void PlayerControllerComponent::onStart() {
    Vec3 f = transform().forward();
    if (f.lengthSqrt() > 0.0001f) {
        f = f.normalized();
        _yaw = std::atan2(f.x, f.z);
        _pitch = std::asin(clamp(f.y, -1.0f, 1.0f));
    }
    else {
        _yaw = 0.0f;
        _pitch = 0.0f;
    }
}

void PlayerControllerComponent::onUpdate(float dt) {
    if (!window || !_owner) return;

    // ------------------- Mouse look -------------------
    float dx = window->mouseDeltaX;
    float dy = window->mouseDeltaY;
    window->mouseDeltaX = 0;
    window->mouseDeltaY = 0;

    _yaw += dx * _mouseSensitivity;
    _pitch += dy * _mouseSensitivity;

    _pitch = clamp(_pitch, -1.5f, 1.5f);

    Quaternion qYaw = Quaternion::fromAxisAngle(Vec3(0, 1, 0), _yaw);
    Vec3 localRight = qYaw.rotate(Vec3(1, 0, 0));
    Quaternion qPitch = Quaternion::fromAxisAngle(localRight, _pitch);

    transform().rotation = (qPitch * qYaw).normalized();

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
    // apply movement
    transform().position += moveDir * (_moveSpeed * dt);

    // collision check
    auto* selfCollider = _owner->getComponent<ColliderComponent>();
    if (selfCollider && selfCollider->enabled()) {

        for (auto* obj : scene()->objects()) {
            if (obj == _owner) continue;

            auto* otherCollider = obj->getComponent<ColliderComponent>();
            if (!otherCollider || !otherCollider->enabled()) continue;

            // If intersect, set position to old pos
            if (selfCollider->intersect(otherCollider)) {
                transform().position = oldPos;
                break;
            }
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