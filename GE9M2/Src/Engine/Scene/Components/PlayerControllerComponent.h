#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Foundation/Transform.h"
#include "../../Platform/Window/Window.h"

class PlayerControllerComponent : public Component {
public:
    float moveSpeed = 20.0f;
    float mouseSensitivity = 0.002f;

    float pitch = 0.0f;
    float yaw = 0.0f;

    int getUpdateOrder() const override { return 10; }

    void onStart() override {
        Vec3 f = owner->transform.forward();
        if (f.lengthSqrt() > 0.0001f) {
            f = f.normalized();
            yaw = std::atan2(f.x, f.z); 
            pitch = std::asin(clamp(f.y, -1.0f, 1.0f)); 
        }
        else {
            yaw = 0.0f;
            pitch = 0.0f;
        }
    }

    void onUpdate(float dt) override {
        if (!window || !owner) return;

		// Mouse look
        float dx = window->mouseDeltaX;
        float dy = window->mouseDeltaY;
        window->mouseDeltaX = 0;
        window->mouseDeltaY = 0;

        yaw += dx * mouseSensitivity;
        pitch -= dy * mouseSensitivity;

        pitch = clamp(pitch, -1.5f, 1.5f);

        Quaternion qYaw = Quaternion::fromAxisAngle(Vec3(0, 1, 0), yaw);
        Vec3 localRight = qYaw.rotate(Vec3(1, 0, 0));
        Quaternion qPitch = Quaternion::fromAxisAngle(localRight, pitch);

        owner->transform.rotation = (qPitch * qYaw).normalized();

		// Keyboard movement
        float inputX = 0.0f;
        float inputZ = 0.0f;

        if (window->keys['W']) inputZ += 1.0f;
        if (window->keys['S']) inputZ -= 1.0f;
        if (window->keys['A']) inputX -= 1.0f;
        if (window->keys['D']) inputX += 1.0f;

        // Handle Z Inut
        Vec3 forward = owner->transform.forward();
        forward.y = 0.0f;

		// Handle X Input
        Vec3 right = owner->transform.right();
        right.y = 0.0f;


        Vec3 moveDir = (forward * inputZ + right * inputX).normalized();

        owner->transform.position += moveDir * (moveSpeed * dt);
    }

    const std::string& getName() const override {
        static std::string name = "PlayerControllerComponent";
        return name;
    }
};
