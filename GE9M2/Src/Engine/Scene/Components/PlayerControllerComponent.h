#pragma once
#include "../../Foundation/Maths.h"
#include "../Component.h"
#include "../GameObject.h"
#include "../Scene.h"
#include "../../Platform/Window/Window.h"

struct PlayerInfo {
    Vec3  moveDir   = Vec3(0, 0, 0);
	bool isWalking = true;

    float walkSpeed = 5.0;
    float runSpeed = 10.0f;

    float moveSpeed() const {
        return isWalking ? walkSpeed : runSpeed;
    }
};

class PlayerControllerComponent : public Component {
private:
    Vec3 _targetMoveDir = Vec3(0, 0, 0);

    const float _moveSmoothFactor = 0.15f;

public:
    PlayerInfo info;

    void onUpdate(float dt) override {
        Vec3 rawInputMoveDir = Vec3(0, 0, 0);

        Vec3 forward = owner->transform.forward();
        Vec3 right = owner->transform.right();
        forward.y = 0;
        right.y = 0;
        forward = forward.normalized();
        right = right.normalized();

        if (window->keys['W']) rawInputMoveDir += forward;
        if (window->keys['S']) rawInputMoveDir -= forward;
        if (window->keys['A']) rawInputMoveDir -= right;
        if (window->keys['D']) rawInputMoveDir += right;

        if (rawInputMoveDir.lengthSqrt() > 0.0f) {
            _targetMoveDir = rawInputMoveDir.normalized();
        }
        else {
            _targetMoveDir = Vec3(0, 0, 0);
        }

        if (window->keys[VK_LSHIFT] && window->keys['W']) {
            info.isWalking = false;
        } else {
            info.isWalking = true;
        }

		// Use linear interpolation to smoothly move towards target move direction
        info.moveDir = info.moveDir + (_targetMoveDir - info.moveDir) * _moveSmoothFactor;
    }

public:

    PlayerControllerComponent() = default;

    int getUpdateOrder() const override {
        return 10;
    }

    const std::string& getName() const override {
        static std::string name = "PlayerControllerComponent";
        return name;
    }
};
