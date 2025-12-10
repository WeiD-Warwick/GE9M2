#pragma once
#include "../../Foundation/Maths.h"
#include "../Component.h"
#include "../GameObject.h"
#include "../Scene.h"
#include "../../Platform/Window/Window.h"

struct PlayerInfo {
    Vec3  moveDir   = Vec3(0, 0, 0);
    float walkSpeed = 5.0;
};

class PlayerControllerComponent : public Component {

public:
    PlayerInfo info;

    void onUpdate(float dt) override {
        Vec3 moveDir = Vec3(0, 0, 0);

        Vec3 forward = owner->transform.forward();
        Vec3 right = owner->transform.right();
        forward.y = 0;
        right.y = 0;
        forward = forward.normalized();
        right = right.normalized();

        if (window->keys['W']) moveDir += forward;
        if (window->keys['S']) moveDir -= forward;
        if (window->keys['A']) moveDir -= right;
        if (window->keys['D']) moveDir += right;

		info.moveDir = moveDir.normalized();
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
