#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../../Platform/Window/Window.h"

class MouseLookComponent : public Component {

private:
    float _sensitivity = 0.004f;
    float _pitch = 0.0f;           // up/down
    float _yaw = 0.0f;             // left/right

    Quaternion _targetRotation = Quaternion::Identity();

    const float _rotateSmoothFactor = 0.2f;

public:

    void onUpdate(float dt) override {
        if (!window) return;

        float dx = window->mouseDeltaX;
        float dy = window->mouseDeltaY;
        window->mouseDeltaX = 0;
        window->mouseDeltaY = 0;

        _yaw += dx * _sensitivity;
        _pitch += dy * _sensitivity;
        _pitch = clamp(_pitch, -1.5f, 1.5f);

        Quaternion qYaw = Quaternion::fromAxisAngle(Vec3(0, 1, 0), _yaw);
        Quaternion qPitch = Quaternion::fromAxisAngle(Vec3(1, 0, 0), _pitch);

        _targetRotation = qYaw * qPitch;
        
		// Use slerp to smoothly rotate towards target rotation
        Quaternion _newRotation = Quaternion::slerp(owner->transform.rotation, _targetRotation, _rotateSmoothFactor);

        owner->transform.rotation = _newRotation;
    }

public:

	MouseLookComponent() = default;

    int getUpdateOrder() const override {
        return 20;
    }

    const std::string& getName() const override {
        static std::string name = "MouseLookComponent";
        return name;
    }
};