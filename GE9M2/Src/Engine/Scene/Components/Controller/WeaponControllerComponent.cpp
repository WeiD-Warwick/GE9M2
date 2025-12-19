#include "WeaponControllerComponent.h"
#include "../../GameObject.h"
#include "../Animator/AnimatorComponent.h"

WeaponControllerComponent::WeaponControllerComponent(const WeaponAnimConfig& config) 
    : _animConfig(config) {}

void WeaponControllerComponent::setIntent(Intent intent) {
    // Only accept new intent when idle
    if (_state != State::Idle)
        return;

    if (_intent == Intent::None)
        _intent = intent;
}

void WeaponControllerComponent::onUpdate(float dt) {
    auto* animator = _owner->getComponent<AnimatorComponent>();
    if (!animator) return;

    // Init default animation
    if (!_idleInitialized) {
        animator->play(_animConfig.idle, true);
        _state = State::Idle;
        _idleInitialized = true;
        return;
    }

    switch (_state) {
    case State::Idle:
        if (_intent == Intent::Fire) {
            animator->play(_animConfig.fire, true);
            _state = State::Firing;
        }
        else if (_intent == Intent::Reload) {
            animator->play(_animConfig.reload, true);
            _state = State::Reloading;
        }
        _intent = Intent::None;
        break;

    case State::Firing:
        if (animator->finished()) {
            animator->play(_animConfig.idle);
            _state = State::Idle;
        }
        break;

    case State::Reloading:
        if (animator->finished()) {
            animator->play(_animConfig.idle);
            _state = State::Idle;
        }
        break;
    }
}
