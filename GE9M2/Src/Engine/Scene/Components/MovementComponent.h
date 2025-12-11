#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "PlayerControllerComponent.h"

class MovementComponent : public Component {

public:

    void onUpdate(float dt) override {
        auto controller = owner->getComponent<PlayerControllerComponent>();
        if (!controller) return;

		Vec3 moveDir = controller->info.moveDir;
		float speed = controller->info.moveSpeed();
        Vec3 desiredMove = moveDir * speed * dt;

		owner->transform.position += desiredMove;
    }

public:
    MovementComponent() = default;

    int getUpdateOrder() const override {
        return 30;
    }

    const std::string& getName() const override {
        static std::string name = "MovementComponent";
        return name;
    }

};
